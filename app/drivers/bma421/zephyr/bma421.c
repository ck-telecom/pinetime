/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT bosch_bma4xx

#include <device.h>
#include <drivers/i2c.h>
#include <init.h>
#include <drivers/sensor.h>
#include <sys/__assert.h>
#include <logging/log.h>

#include "bma4.h"
#include "bma421_features.h"

#include "bma421.h"

LOG_MODULE_REGISTER(BMA421, CONFIG_SENSOR_LOG_LEVEL);

static int8_t user_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t length, void* intf_ptr)
{
	struct device *dev = intf_ptr;
	const struct bma421_config *cfg = dev->config;
	struct bma421_data *drv_data = dev->data;

	i2c_burst_read(drv_data->i2c, cfg->i2c_addr, reg_addr, reg_data, length);
	return 0;
}

static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t length, void* intf_ptr)
{
	struct device *dev = intf_ptr;
	const struct bma421_config *cfg = dev->config;
	struct bma421_data *drv_data = dev->data;

	i2c_burst_write(drv_data->i2c, cfg->i2c_addr, reg_addr, reg_data, length);
	return 0;
}

static void user_delay(uint32_t period_us, void* intf_ptr)
{
	k_busy_wait(period_us);
}

static int bma421_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct bma421_config *cfg = dev->config;
	struct bma421_data *drv_data = dev->data;
	uint8_t buf[6] = { 0 };
	uint16_t lsb = 0U;
	uint16_t msb = 0U;

	__ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL);



	msb = buf[1];
	lsb = buf[0];
	drv_data->x_sample = (uint16_t)(msb << 8) | lsb;

	msb = buf[3];
	lsb = buf[2];
	drv_data->y_sample = (uint16_t)(msb << 8) | lsb;

	msb = buf[5];
	lsb = buf[4];
	drv_data->z_sample = (uint16_t)(msb << 8) | lsb;
/*
	if (i2c_reg_read_byte(drv_data->i2c, BMA421_I2C_ADDRESS,
				BMA421_REG_TEMP,
				(uint8_t *)&drv_data->temp_sample) < 0) {
		LOG_DBG("Could not read temperature data");
		return -EIO;
	}*/
	return 0;
}

static void bma421_channel_accel_convert(struct sensor_value *val,
		uint16_t raw_val)
{
	/*
	 * accel_val = (sample * BMA280_PMU_FULL_RAGE) /
	 *             (2^data_width * 10^6)
	 */
/*	raw_val = (raw_val * BMA421_ACC_FULL_RANGE) /
		(1 << (8 + BMA421_ACCEL_LSB_BITS));
	val->val1 = raw_val / 1000000;
	val->val2 = raw_val % 1000000;*/

	/* normalize val to make sure val->val2 is positive */
/*	if (val->val2 < 0) {
		val->val1 -= 1;
		val->val2 += 1000000;
	}*/
}

static void bma421_channel_value_add(struct sensor_value *val)
{
	val->val1 = 32; //todo -- here values can be read from REG 0x1E step counter
	val->val2 = 88;
}

static int bma421_channel_get(const struct device *dev,
		enum sensor_channel chan,
		struct sensor_value *val)
{
	struct bma421_data *drv_data = dev->data;

	/*
	 * See datasheet "Sensor data" section for
	 * more details on processing sample data.
	 */
	if (chan == SENSOR_CHAN_ACCEL_X) {
		bma421_channel_accel_convert(val, drv_data->x_sample);
	} else if (chan == SENSOR_CHAN_ACCEL_Y) {
		bma421_channel_accel_convert(val, drv_data->y_sample);
	} else if (chan == SENSOR_CHAN_ACCEL_Z) {
		bma421_channel_accel_convert(val, drv_data->z_sample);
	} else if (chan == SENSOR_CHAN_ACCEL_XYZ) {
		bma421_channel_accel_convert(val, drv_data->x_sample);
		bma421_channel_accel_convert(val + 1, drv_data->y_sample);
		bma421_channel_accel_convert(val + 2, drv_data->z_sample);
		bma421_channel_value_add(val + 3); //todo check how extra data can be passed
	} else if (chan == SENSOR_CHAN_DIE_TEMP) {
		/* temperature_val = 23 + sample / 2 */
		val->val1 = (drv_data->temp_sample >> 1) + 23;
		val->val2 = 500000 * (drv_data->temp_sample & 1);
		return 0;
	} else {
		return -ENOTSUP;
	}

	return 0;
}

static const struct sensor_driver_api bma421_driver_api = {
#if CONFIG_BMA421_TRIGGER
	.attr_set = bma421_attr_set,
	.trigger_set = bma421_trigger_set,
#endif
	.sample_fetch = bma421_sample_fetch,
	.channel_get = bma421_channel_get,
};

int bma421_init_driver(const struct device *dev)
{
	const struct bma421_config *cfg = dev->config;
	struct bma421_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;
	int8_t ret = 0;
    uint8_t id = 0;
	drv_data->i2c = device_get_binding(cfg->i2c_bus);
	if (drv_data->i2c == NULL) {
		LOG_ERR("Could not get pointer to %s device",
				cfg->i2c_bus);
		return -EINVAL;
	}

	bma_dev->intf = BMA4_I2C_INTF;
	bma_dev->bus_read = user_i2c_read;
	bma_dev->bus_write = user_i2c_write;
	bma_dev->variant = BMA42X_VARIANT;
	bma_dev->intf_ptr = dev;
	bma_dev->delay_us = user_delay;
	bma_dev->read_write_len = 8;

	ret = bma4_soft_reset(bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("BMA4 soft reset error:%d", ret);
		return ret;
	}
	k_busy_wait(1000);

	ret = bma421_init(bma_dev);
	if (ret != BMA4_OK) {
	LOG_ERR("BMA4 init error:%d", ret);
		return ret;
    }


	ret = bma4_set_accel_enable(1, bma_dev);
	if (ret != BMA4_OK)
		return ret;

	struct bma4_accel_config accel_conf;
	accel_conf.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
	accel_conf.range = BMA4_ACCEL_RANGE_2G;
	accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
	accel_conf.perf_mode = BMA4_CONTINUOUS_MODE;
	ret = bma4_set_accel_config(&accel_conf, bma_dev);
	if (ret != BMA4_OK)
		return ret;

#ifdef CONFIG_BMA421_TRIGGER
	if (bma421_init_interrupt(dev) < 0) {
		LOG_DBG("Could not initialize interrupts");
		return -EIO;
	}
#endif
    LOG_INF("bma421 init done");
	return 0;
}

struct bma421_data bma421_driver;

static const struct bma421_config bma421_cfg = {
	.i2c_bus = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
#if CONFIG_BMA421_TRIGGER
	.drdy_pin = DT_INST_GPIO_PIN(0, int1_gpios),
	.drdy_flags = DT_INST_GPIO_FLAGS(0, int1_gpios),
	.drdy_controller = DT_INST_GPIO_LABEL(0, int1_gpios),
#endif
};

DEVICE_DT_INST_DEFINE(0, bma421_init_driver, NULL,
			&bma421_driver, &bma421_cfg, POST_KERNEL,
			CONFIG_SENSOR_INIT_PRIORITY, &bma421_driver_api);