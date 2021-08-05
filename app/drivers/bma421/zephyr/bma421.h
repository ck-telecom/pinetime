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

#define BMA421_REG_CHIP_ID		0x00
#if CONFIG_BMA421_CHIP_BMA421
#define BMA421_CHIP_ID		0x11
#endif

#define BMA421_REG_ERROR                0x02
#define BMA421_REG_STATUS               0x03
#define BMA421_REG_ACC_DATA0            0x0A
#define BMA421_REG_ACC_DATA8            0x12
#define BMA421_REG_SENSORTIME_0         0x18
#define BMA421_REG_STAT_0               0x1C
#define BMA421_REG_STAT_1               0x1D
#define BMA421_REG_STEP_CNT_OUT_0       0x1E
#define BMA421_REG_HIGH_G_OUT           0x1F
#define BMA421_REG_TEMPERATURE          0x22
#define BMA421_REG_FIFO_LENGTH_0        0x24
#define BMA421_REG_FIFO_DATA            0x26
#define BMA421_REG_ACTIVITY_OUT         0x27
#define BMA421_REG_ORIENTATION_OUT      0x28
#define BMA421_REG_INTERNAL_STAT        0x2A
#define BMA421_REG_ACC_CONF             0x40
#define BMA421_REG_ACC_RANGE            0x41
#define BMA421_REG_AUX_CONFIG           0x44
#define BMA421_REG_FIFO_DOWN            0x45
#define BMA421_REG_FIFO_WTM_0           0x46
#define BMA421_REG_FIFO_CONFIG_0        0x48
#define BMA421_REG_FIFO_CONFIG_1        0x49
#define BMA421_REG_AUX_DEV_ID           0x4B
#define BMA421_REG_AUX_IF_CONF          0x4C
#define BMA421_REG_AUX_RD               0x4D
#define BMA421_REG_AUX_WR               0x4E
#define BMA421_REG_AUX_DATA             0x4F
#define BMA421_REG_INT1_IO_CTRL         0x53
#define BMA421_REG_INT2_IO_CTRL         0x54
#define BMA421_REG_INTR_LATCH           0x55
#define BMA421_REG_INT_MAP_1            0x56
#define BMA421_REG_INT_MAP_2            0x57
#define BMA421_REG_INT_MAP_DATA         0x58
#define BMA421_REG_INIT_CTRL            0x59
#define BMA421_REG_ASIC_LSB             0x5B
#define BMA421_REG_ASIC_MSB             0x5C
#define BMA421_REG_FEATURE_CONFIG       0x5E
#define BMA421_REG_IF_CONFIG            0x6B
#define BMA421_REG_ACC_SELF_TEST        0x6D
#define BMA421_REG_NV_CONFIG            0x70
#define BMA421_REG_OFFSET_0             0x71
#define BMA421_REG_OFFSET_1             0x72
#define BMA421_REG_OFFSET_2             0x73
#define BMA421_REG_PWR_CONF             0x7C
#define BMA421_REG_PWR_CTRL             0x7D
#define BMA421_REG_CMD                  0x7E

//#define BMA421_FEATURE_SIZE                    UINT8_C(64)
#define BMA421_FEATURE_SIZE                    64

#define BMA421_CMD_SOFT_RESET        0xB6
#define BMA421_CMD_SOFT_RESET_MASK   0xB6

#define BMA421_ACC_PERF_MODE         0x80 //continues filter function

//#define BMA421_REG_PMU_BW		0x10
#define BMA421_ACC_ODR_MASK     0x0F

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

#define BMA4_ACCEL_RANGE_MSK    0x03

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

#define BMA4_ACCEL_ENABLE_MSK   0x04

#define BMA4_ACCEL_PERFMODE_MSK     0x80
#define BMA4_ACCEL_PERFMODE_DATA    0x80

#define BMA421_THREAD_PRIORITY          10
#define BMA421_THREAD_STACKSIZE_UNIT    1024

struct bma421_data {
	struct device *i2c;
	uint16_t x_sample;
	uint16_t y_sample;
	uint16_t z_sample;
	uint8_t temp_sample;

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
int bma421_trigger_set(struct device *dev,
		const struct sensor_trigger *trig,
		sensor_trigger_handler_t handler);

int bma421_attr_set(struct device *dev,
		enum sensor_channel chan,
		enum sensor_attribute attr,
		const struct sensor_value *val);

int bma421_init_interrupt(struct device *dev);

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
