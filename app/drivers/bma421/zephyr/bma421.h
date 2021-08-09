/*
 *
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_BMA421_BMA421_H_
#define ZEPHYR_DRIVERS_SENSOR_BMA421_BMA421_H_

#include <device.h>
#include <sys/util.h>
#include <zephyr/types.h>
#include <drivers/gpio.h>

#include "bma4_defs.h"


#if CONFIG_BMA421_ACC_ODR_1
#define BMA421_ACC_ODR		0x01
#elif CONFIG_BMA421_ACC_ODR_2
#define BMA421_ACC_ODR		0x02
#elif CONFIG_BMA421_ACC_ODR_3
#define BMA421_ACC_ODR		0x03
#elif CONFIG_BMA421_ACC_ODR_4
#define BMA421_ACC_ODR		0x04
#elif CONFIG_BMA421_ACC_ODR_5
#define BMA421_ACC_ODR		0x05
#elif CONFIG_BMA421_ACC_ODR_6
#define BMA421_ACC_ODR		0x06
#elif CONFIG_BMA421_ACC_ODR_7
#define BMA421_ACC_ODR		0x07
#elif CONFIG_BMA421_ACC_ODR_8
#define BMA421_ACC_ODR		0x08
#elif CONFIG_BMA421_ACC_ODR_9
#define BMA421_ACC_ODR		0x09
#elif CONFIG_BMA421_ACC_ODR_10
#define BMA421_ACC_ODR		0x0a
#elif CONFIG_BMA421_ACC_ODR_11
#define BMA421_ACC_ODR		0x0b
#elif CONFIG_BMA421_ACC_ODR_12
#define BMA421_ACC_ODR		0x0c
#elif CONFIG_BMA421_ACC_ODR_13
#define BMA421_ACC_ODR		0x00
#endif


#if CONFIG_BMA421_ACC_RANGE_2G
#define BMA421_ACC_RANGE	0x00
#define BMA421_ACC_FULL_RANGE	(4 * SENSOR_G)
#elif CONFIG_BMA421_ACC_RANGE_4G
#define BMA421_ACC_RANGE	0x01
#define BMA421_ACC_FULL_RANGE	(8 * SENSOR_G)
#elif CONFIG_BMA421_ACC_RANGE_8G
#define BMA421_ACC_RANGE	0x02
#define BMA421_ACC_FULL_RANGE	(16 * SENSOR_G)
#elif CONFIG_BMA421_ACC_RANGE_16G
#define BMA421_ACC_RANGE	0x03
#define BMA421_ACC_FULL_RANGE	(32 * SENSOR_G)
#endif


#define BMA421_THREAD_PRIORITY          10
#define BMA421_THREAD_STACKSIZE_UNIT    1024

struct bma421_data {
	struct device *i2c;
	uint16_t x_sample;
	uint16_t y_sample;
	uint16_t z_sample;
	uint8_t temp_sample;
	struct bma4_dev bma_dev;
#ifdef CONFIG_BMA421_TRIGGER
	const struct device *gpio;
	struct gpio_callback gpio_cb;

	struct sensor_trigger data_ready_trigger;
	sensor_trigger_handler_t data_ready_handler;

	struct sensor_trigger any_motion_trigger;
	sensor_trigger_handler_t any_motion_handler;

#if defined(CONFIG_BMA421_TRIGGER_OWN_THREAD)
	K_THREAD_STACK_MEMBER(thread_stack, CONFIG_BMA421_THREAD_STACK_SIZE);
	struct k_thread thread;
	struct k_sem gpio_sem;
#elif defined(CONFIG_BMA421_TRIGGER_GLOBAL_THREAD)
	struct k_work work;
	struct device *dev;
#endif

#endif /* CONFIG_BMA421_TRIGGER */
};

#ifdef CONFIG_BMA421_TRIGGER
int bma421_trigger_set(const struct device *dev,
		const struct sensor_trigger *trig,
		sensor_trigger_handler_t handler);

int bma421_attr_set(const struct device *dev,
		enum sensor_channel chan,
		enum sensor_attribute attr,
		const struct sensor_value *val);

int bma421_init_interrupt(const struct device *dev);

#endif

struct bma421_config {
	const char *i2c_bus;
	uint16_t i2c_addr;
#if CONFIG_BMA421_TRIGGER
	gpio_pin_t drdy_pin;
	gpio_flags_t drdy_flags;
	const char *drdy_controller;
#endif /* CONFIG_BMA421_TRIGGER */
};

#endif /* ZEPHYR_DRIVERS_SENSOR_BMA421_BMA421_H_ */
