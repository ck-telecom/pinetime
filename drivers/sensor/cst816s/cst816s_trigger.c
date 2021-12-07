/*
 * Copyright (c) 2020 Stephane Dorre <stephane.dorre@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT hynitron_cst816s

#include <device.h>
#include <drivers/i2c.h>
#include <sys/util.h>
#include <kernel.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "cst816s.h"

LOG_MODULE_DECLARE(CST816S, CONFIG_SENSOR_LOG_LEVEL);

#ifdef CONFIG_CST816S_TRIGGER
static void cst816s_gpio_callback(const struct device *dev,
		struct gpio_callback *cb, uint32_t pins)
{
	struct cst816s_data *drv_data =
		CONTAINER_OF(cb, struct cst816s_data, gpio_cb);

	ARG_UNUSED(pins);

#if defined(CONFIG_CST816S_TRIGGER_OWN_THREAD)
	k_sem_give(&drv_data->gpio_sem);
#elif defined(CONFIG_CST816S_TRIGGER_GLOBAL_THREAD)
	k_work_submit(&drv_data->work);
#endif
}

static void cst816s_thread_cb(const struct device *dev)
{
	struct cst816s_data *drv_data = dev->data;

	if (drv_data->data_ready_handler != NULL) {
		drv_data->data_ready_handler(dev, &drv_data->data_ready_trigger);
	}
}

#ifdef CONFIG_CST816S_TRIGGER_OWN_THREAD
static void cst816s_thread(struct cst816s_data *drv_data)
{
	while (true) {
		k_sem_take(&drv_data->gpio_sem, K_FOREVER);
		cst816s_thread_cb(drv_data->dev);
	}
}
#endif

#ifdef CONFIG_CST816S_TRIGGER_GLOBAL_THREAD
static void cst816s_work_cb(struct k_work *work)
{
	struct cst816s_data *drv_data =
		CONTAINER_OF(work, struct cst816s_data, work);

	cst816s_thread_cb(drv_data->dev);
}
#endif

int cst816s_trigger_set(const struct device *dev,
		const struct sensor_trigger *trig,
		sensor_trigger_handler_t handler)
{
	struct cst816s_data *drv_data = dev->data;

	if (trig->type == SENSOR_TRIG_DATA_READY) {

		drv_data->data_ready_handler = handler;
		if (handler == NULL) {
			return 0;
		}
		drv_data->data_ready_trigger = *trig;
	}
	else {
		return -ENOTSUP;
	}

	return 0;
}

int cst816s_init_interrupt(const struct device *dev)
{
	struct cst816s_data *drv_data = dev->data;
	const struct cst816s_config *cfg = dev->config;
	int ret;

	/* setup data ready gpio interrupt */
	drv_data->gpio = device_get_binding(DT_INST_GPIO_LABEL(0, irq_gpios));
	if (drv_data->gpio == NULL) {
		LOG_ERR("Cannot get pointer to %s device",
				DT_INST_GPIO_LABEL(0, irq_gpios));
		return -EINVAL;
	}

	ret = gpio_pin_configure(drv_data->gpio, cfg->drdy_pin,
				GPIO_INPUT | cfg->drdy_flags);
	if (ret) {
		return ret;
	}

	gpio_init_callback(&drv_data->gpio_cb,
			cst816s_gpio_callback,
			BIT(cfg->drdy_pin));

	ret = gpio_add_callback(drv_data->gpio, &drv_data->gpio_cb);
	if (ret < 0) {
		LOG_ERR("Could not set gpio callback");
		return ret;
	}

	ret = gpio_pin_interrupt_configure(drv_data->gpio,
			cfg->drdy_pin, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		return ret;
	}

#if defined(CONFIG_CST816S_TRIGGER_OWN_THREAD)
	k_sem_init(&drv_data->gpio_sem, 0, UINT_MAX);

	k_thread_create(&drv_data->thread, drv_data->thread_stack,
			CONFIG_CST816S_THREAD_STACK_SIZE,
			(k_thread_entry_t)cst816s_thread, drv_data,
			NULL, NULL, K_PRIO_COOP(CONFIG_CST816S_THREAD_PRIORITY),
			0, K_NO_WAIT);
#elif defined(CONFIG_CST816S_TRIGGER_GLOBAL_THREAD)
	drv_data->work.handler = cst816s_work_cb;
	drv_data->dev = dev;
#endif
	return ret;
}
#endif /* CONFIG_CST816S_TRIGGER */
