/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/i2c.h>
#include <sys/util.h>
#include <kernel.h>
#include <drivers/sensor.h>
#include <logging/log.h>
#include <stdbool.h>

#include "bma421.h"
#include "bma421/bma421.h"

LOG_MODULE_DECLARE(BMA421, CONFIG_SENSOR_LOG_LEVEL);

#ifdef CONFIG_BMA421_TRIGGER
static void bma421_gpio_callback(const struct device *dev,
				 struct gpio_callback *cb, uint32_t pins)
{
	struct bma421_data *drv_data = dev->data;

	ARG_UNUSED(pins);

#if defined(CONFIG_BMA421_TRIGGER_OWN_THREAD)
	k_sem_give(&drv_data->gpio_sem);
#elif defined(CONFIG_BMA421_TRIGGER_GLOBAL_THREAD)
	k_work_submit(&drv_data->work);
#endif
}

static void bma421_thread_cb(const struct device *dev)
{
	struct bma421_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;

	uint16_t int_status = 0xffffu;
	bma421_read_int_status(&int_status, bma_dev);
LOG_INF("int status:0x%x", int_status);

	/* check for data ready */
	if (((int_status & BMA4_ACCEL_DATA_RDY_INT) == BMA4_ACCEL_DATA_RDY_INT)
		&& drv_data->data_ready_handler != NULL) {
		drv_data->data_ready_handler(dev, &drv_data->data_ready_trigger);
	}

	/* check for any error */
	if (((int_status & BMA421_ERROR_INT) == BMA421_ERROR_INT)) {
		LOG_ERR("Interrupt status 0x%x - Error detected!", int_status);
		// TODO: Handle error (maybe soft reset ?)
	}
}

#ifdef CONFIG_BMA421_TRIGGER_OWN_THREAD
static void bma421_thread(int dev_ptr, int unused)
{
	struct device *dev = INT_TO_POINTER(dev_ptr);
	struct bma421_data *drv_data = dev->data;

	ARG_UNUSED(unused);

	while (1) {
		k_sem_take(&drv_data->gpio_sem, K_FOREVER);
		bma421_thread_cb(dev);
	}
}
#endif

#ifdef CONFIG_BMA421_TRIGGER_GLOBAL_THREAD
static void bma421_work_cb(struct k_work *work)
{
	struct bma421_data *drv_data =
		CONTAINER_OF(work, struct bma421_data, work);

	bma421_thread_cb(drv_data->dev);
}
#endif

int bma421_trigger_set(const struct device *dev,
			const struct sensor_trigger *trig,
			sensor_trigger_handler_t handler)
{
//	const struct bma421_config *cfg = dev->config;
	struct bma421_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;
	int8_t ret;
	uint16_t interrupt_mask = 0;
	uint8_t interrupt_enable = BMA4_ENABLE;

	if (handler == NULL) {
		interrupt_enable = BMA4_DISABLE;
	}

	switch (trig->type) {
	case SENSOR_TRIG_DATA_READY:
		interrupt_mask = BMA4_DATA_RDY_INT;
		drv_data->data_ready_handler = handler;
		drv_data->data_ready_trigger = *trig;
		break;
	default:
		LOG_ERR("Unsupported sensor trigger");
		return -ENOTSUP;
	}

	// Add Error interrupt in any case.
	interrupt_mask |= BMA421_ERROR_INT;

	ret = bma421_map_interrupt(BMA4_INTR1_MAP, interrupt_mask, interrupt_enable, bma_dev);
	if (ret) {
		LOG_ERR("Map interrupt failed err %d", ret);
		return ret;
	}

	uint16_t int_status = 0xffffu;
	bma421_read_int_status(&int_status, bma_dev);
	LOG_WRN("Reading Interrupt status 0x%x", int_status);

	return 0;
}

int bma421_init_interrupt(const struct device *dev)
{
	const struct bma421_config *cfg = dev->config;
	struct bma421_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;
	int8_t ret;

	if (!device_is_ready(cfg->int1_gpio.port)) {
		LOG_ERR("GPIO device %s not ready", cfg->int1_gpio.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&cfg->int1_gpio, GPIO_INPUT);
	if (ret < 0) {
		return ret;
	}

	gpio_init_callback(&drv_data->gpio_cb,
			bma421_gpio_callback,
			BIT(cfg->int1_gpio.pin));

	ret = gpio_add_callback(cfg->int1_gpio.port, &drv_data->gpio_cb);
	if (ret < 0) {
		LOG_ERR("Could not set gpio callback");
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&cfg->int1_gpio, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		return ret;
	}

	uint16_t int_status = 0xffffu;
	bma421_read_int_status(&int_status, bma_dev);
	LOG_WRN("Interrupt status 0x%x", int_status);

	struct bma4_int_pin_config pin_config;
	bma4_get_int_pin_config(&pin_config, BMA4_INTR1_MAP, bma_dev);

	pin_config.output_en = BMA4_OUTPUT_ENABLE;
	pin_config.od = BMA4_OPEN_DRAIN;
	pin_config.lvl = BMA4_ACTIVE_LOW;
	pin_config.edge_ctrl = BMA4_EDGE_TRIGGER;
	/* .edge_ctrl and .input_en are for input interrupt configuration */

	ret = bma4_set_int_pin_config(&pin_config, BMA4_INTR1_MAP, bma_dev);
	if (ret) {
		LOG_ERR("Set interrupt config err %d", ret);
		return ret;
	}

	/* Latch mode means that interrupt flag are only reset once the status is read */
	bma4_set_interrupt_mode(BMA4_LATCH_MODE, bma_dev);

	uint8_t bma_status = 0xffu;
	bma4_get_status(&bma_status, bma_dev);

#if defined(CONFIG_BMA421_TRIGGER_OWN_THREAD)
	k_sem_init(&drv_data->gpio_sem, 0, UINT_MAX);

	k_thread_create(&drv_data->thread, drv_data->thread_stack,
			CONFIG_BMA421_THREAD_STACK_SIZE,
			(k_thread_entry_t)bma421_thread, dev,
			0, NULL, K_PRIO_COOP(CONFIG_BMA421_THREAD_PRIORITY),
			0, K_NO_WAIT);
#elif defined(CONFIG_BMA421_TRIGGER_GLOBAL_THREAD)
	drv_data->work.handler = bma421_work_cb;
	drv_data->dev = dev;
#endif
	LOG_INF("bma421 trigger init done");
	return ret;
}

#endif
