/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Stephane Dorre <stephane.dorre@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT hynitron_cst816s

#include <device.h>
#include <drivers/i2c.h>
#include <init.h>
#include <drivers/sensor.h>
#include <sys/__assert.h>
#include <logging/log.h>

#include "cst816s.h"

#define RESET_PIN DT_INST_GPIO_PIN(0, rst_gpios)
#define RESET_FLAGS DT_INST_GPIO_FLAGS(0, rst_gpios)

LOG_MODULE_REGISTER(CST816S, CONFIG_SENSOR_LOG_LEVEL);

static int cst816s_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct cst816s_config *cfg = dev->config;
	struct cst816s_data *drv_data = dev->data;
	uint8_t buf[9];
	uint8_t msb;
	uint8_t lsb;
	__ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL);

	/*
	 * since all accel data register addresses are consecutive,
	 * a burst read can be used to read all the samples
	 */
	if (i2c_burst_read(drv_data->i2c, cfg->i2c_addr,
				CST816S_REG_DATA, buf, 9) < 0) {
		LOG_ERR("Could not read data");
		return -EIO;
	}

	switch (buf[CST816S_REG_GESTURE_ID])  {
		case CST816S_GESTURE_NONE:
			drv_data->gesture = NONE;
			break;
		case CST816S_GESTURE_SLIDE_UP:
			drv_data->gesture = SLIDE_UP;
			break;
		case CST816S_GESTURE_SLIDE_DOWN:
			drv_data->gesture = SLIDE_DOWN;
			break;
		case CST816S_GESTURE_SLIDE_LEFT:
			drv_data->gesture = SLIDE_LEFT;
			break;
		case CST816S_GESTURE_SLIDE_RIGHT:
			drv_data->gesture = SLIDE_RIGHT;
			break;
		case CST816S_GESTURE_CLICK:
			drv_data->gesture = CLICK;
			break;
		case CST816S_GESTURE_DOUBLE_CLICK:
			drv_data->gesture = DOUBLE_CLICK;
			break;
		case CST816S_GESTURE_LONG_PRESS:
			drv_data->gesture = LONG_PRESS;
			break;
	}

	drv_data->number_touch_point = buf[CST816S_REG_FINGER_NUM];

	msb = buf[CST816S_REG_XPOS_H] & 0x0f;
	lsb = buf[CST816S_REG_XPOS_L];
	drv_data->touch_point_1.x = (msb<<8)|lsb;

	msb = buf[CST816S_REG_YPOS_H] & 0x0f;
	lsb = buf[CST816S_REG_YPOS_L];
	drv_data->touch_point_1.y = (msb<<8)|lsb;

	drv_data->action = (enum cst816s_action)(buf[CST816S_REG_XPOS_H] >> 6);
	return 0;
}

static int cst816s_channel_get(const struct device *dev,
		enum sensor_channel chan,
		struct sensor_value *val)
{
	struct cst816s_data *drv_data = dev->data;

	if ((uint16_t)chan == CST816S_CHAN_GESTURE) {
		val->val1=drv_data->gesture;
	} else if ((uint16_t)chan == CST816S_CHAN_TOUCH_POINT_1) {
		val->val1=drv_data->touch_point_1.x;
		val->val2=drv_data->touch_point_1.y;
	} else if ((uint16_t)chan == CST816S_CHAN_TOUCH_POINT_2) {
		val->val1=drv_data->touch_point_2.x;
		val->val2=drv_data->touch_point_2.y;
	} else {
		return -ENOTSUP;
	}

	return 0;
}

static const struct sensor_driver_api cst816s_driver_api = {
#if CONFIG_CST816S_TRIGGER
	.attr_set = cst816s_attr_set,
	.trigger_set = cst816s_trigger_set,
#endif
	.sample_fetch = cst816s_sample_fetch,
	.channel_get = cst816s_channel_get,
};

static void cst816s_chip_reset(const struct device* dev)
{
	struct cst816s_data *drv_data = dev->data;

	gpio_pin_set(drv_data->reset_gpio, RESET_PIN, 0);
	k_msleep(5);
	gpio_pin_set(drv_data->reset_gpio, RESET_PIN, 1);
	k_msleep(50);
}

static int cst816s_chip_init(const struct device *dev)
{
	const struct cst816s_config *cfg = dev->config;
	struct cst816s_data *drv_data = dev->data;

	// cst816s_chip_reset(dev);

	if (i2c_reg_read_byte(drv_data->i2c, cfg->i2c_addr,
			CST816S_REG_CHIP_ID, &drv_data->chip_id) < 0) {
		LOG_ERR("failed reading chip id");
		return -EIO;
	}

	if (drv_data->chip_id != CST816S_CHIP_ID) {
		LOG_ERR("CST816S wrong chip id: returned 0x%x", drv_data->chip_id);
	}

	if (i2c_reg_update_byte(drv_data->i2c, cfg->i2c_addr,
				CST816S_REG_MOTION_MASK,
				(CST816S_MOTION_EN_DCLICK), (CST816S_MOTION_EN_DCLICK)) < 0) {
		LOG_ERR("Could not enable double click");
		return -EIO;
	}

	return 0;
}

int cst816s_init(const struct device *dev)
{
	int retval = 0;
    const struct cst816s_config *cfg = dev->config;
	struct cst816s_data *drv_data = dev->data;

	drv_data->i2c = device_get_binding(cfg->i2c_bus);
	if (drv_data->i2c == NULL) {
		LOG_ERR("Could not get pointer to %s device", cfg->i2c_bus);
		return -EINVAL;
	}
#if DT_INST_NODE_HAS_PROP(0, rst_gpios)
	/* setup reset gpio */
	drv_data->reset_gpio = device_get_binding(
			DT_INST_GPIO_LABEL(0, rst_gpios));
	if (drv_data->reset_gpio == NULL) {
		LOG_ERR("Cannot get pointer to %s device",
		    DT_INST_GPIO_LABEL(0, rst_gpios));
		return -EINVAL;
	}

	/* Configure pin as output and initialize it to high. */
	LOG_INF("configure OUTPUT ACTIVE");
	gpio_pin_configure(drv_data->reset_gpio, RESET_PIN,
			GPIO_OUTPUT | RESET_FLAGS);
#endif
	retval = cst816s_chip_init(dev);
	if (retval != 0) {
		LOG_ERR("cst816s init error");
		return retval;
	}

#ifdef CONFIG_CST816S_TRIGGER
	if (cst816s_init_interrupt(dev) < 0) {
		LOG_DBG("Could not initialize interrupts");
		return -EIO;
	}
#endif
	return 0;
}

static const struct cst816s_config cst816s_cfg = {
	.i2c_bus = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
#if CONFIG_CST816S_TRIGGER
	.drdy_pin = DT_INST_GPIO_PIN(0, irq_gpios),
	.drdy_flags = DT_INST_GPIO_FLAGS(0, irq_gpios),
	.drdy_controller = DT_INST_GPIO_LABEL(0, irq_gpios),
#endif
};

struct cst816s_data cst816s_driver;

DEVICE_DT_INST_DEFINE(0, cst816s_init, NULL,
		&cst816s_driver, &cst816s_cfg, POST_KERNEL,
		CONFIG_SENSOR_INIT_PRIORITY, &cst816s_driver_api);