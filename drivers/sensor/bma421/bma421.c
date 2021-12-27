/*
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT bosch_bma4xx

#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "bma4.h"
#include "bma421/bma421.h"

#include "bma421.h"

LOG_MODULE_REGISTER(BMA421, CONFIG_SENSOR_LOG_LEVEL);

static int8_t user_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t length, void* intf_ptr)
{
	int8_t ret;
	const struct device *dev = intf_ptr;
	const struct bma421_config *cfg = dev->config;

	ret = i2c_burst_read_dt(&cfg->i2c, reg_addr, reg_data, length);
	if (ret < 0) {
		LOG_ERR("i2c_burst_read error %d", ret);
		return ret;
	}
	return 0;
}

static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t length, void* intf_ptr)
{
	int8_t ret;
	const struct device *dev = intf_ptr;
	const struct bma421_config *cfg = dev->config;

	ret = i2c_burst_write_dt(&cfg->i2c, reg_addr, reg_data, length);
	if (ret < 0) {
		LOG_ERR("i2c_burst_write error %d", ret);
		return ret;
	}
	return 0;
}

static void user_delay(uint32_t period_us, void* intf_ptr)
{
	k_busy_wait(period_us);
}

static int bma421_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	int retval = 0;

	struct bma421_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;

	switch (chan) {
	case SENSOR_CHAN_ACCEL_X:
	case SENSOR_CHAN_ACCEL_Y:
	case SENSOR_CHAN_ACCEL_Z:
	case SENSOR_CHAN_ACCEL_XYZ:
		retval = bma4_read_accel_xyz(&drv_data->accel, bma_dev);
		if (retval < 0) {
			LOG_ERR("bma4_read_accel_xyz error: %d", retval);
			return retval;
		};
		break;

	case SENSOR_CHAN_DIE_TEMP:
		retval = bma4_get_temperature(&drv_data->temperature, BMA4_DEG, bma_dev);
		break;

	case SENSOR_CHAN_ALL:
		retval = bma4_read_accel_xyz(&drv_data->accel, bma_dev);
		retval |= bma4_get_temperature(&drv_data->temperature, BMA4_DEG, bma_dev);
 //uint32_t steps = 0;
// bma4_step_counter_output(&steps, &bma);
  //int32_t temperature;
  //bma4_get_temperature(&temperature, BMA4_DEG, &bma);
  //temperature = temperature / 1000;

  //uint8_t activity = 0;
  //bma423_activity_output(&activity, &bma);
		if (retval < 0) {
			LOG_ERR("bma4_read_accel_xyz error: %d", retval);
			return retval;
		}
		break;

	default:
		return -ENOTSUP;
	}

	return retval;
}

static void bma421_channel_accel_convert(struct sensor_value *val,
		int64_t raw_val)
{
	raw_val = (raw_val * BMA421_ACC_FULL_RANGE / 4096);

	val->val1 = raw_val / 1000000LL;
	val->val2 = raw_val % 1000000LL;

	/* normalize val to make sure val->val2 is positive */
	if (val->val2 < 0) {
		val->val1 -= 1;
		val->val2 += 1000000;
	}
}

static int bma421_channel_get(const struct device *dev,
		enum sensor_channel chan,
		struct sensor_value *val)
{
	struct bma421_data *drv_data = dev->data;

	if (chan == SENSOR_CHAN_ACCEL_X) {
		bma421_channel_accel_convert(val, drv_data->accel.x);
	} else if (chan == SENSOR_CHAN_ACCEL_Y) {
		bma421_channel_accel_convert(val, drv_data->accel.y);
	} else if (chan == SENSOR_CHAN_ACCEL_Z) {
		bma421_channel_accel_convert(val, drv_data->accel.z);
	} else if (chan == SENSOR_CHAN_ACCEL_XYZ) {
		bma421_channel_accel_convert(val, drv_data->accel.x);
		bma421_channel_accel_convert(val + 1, drv_data->accel.y);
		bma421_channel_accel_convert(val + 2, drv_data->accel.z);
	} else if (chan == SENSOR_CHAN_DIE_TEMP) {
		/* temperature_val = 23 + sample / 2 */
		val->val1 = (drv_data->temperature >> 1) + 23;
		val->val2 = 500000 * (drv_data->temperature & 1);
		return 0;
	} else {
		return -ENOTSUP;
	}

	return 0;
}

int bma421_attr_set(const struct device *dev,
			enum sensor_channel chan,
			enum sensor_attribute attr,
			const struct sensor_value *val)
{
	int ret;
	struct bma421_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;
	struct bma4_accel_config accel_conf = { 0 };

	if (chan != SENSOR_CHAN_ACCEL_XYZ) {
		return -ENOTSUP;
	}
	switch (attr) {
	case SENSOR_ATTR_SAMPLING_FREQUENCY:
		ret = bma4_get_accel_config(&accel_conf, bma_dev);
		if (ret != BMA4_OK) {
			LOG_ERR("Failed to get Acceleration config err %d", ret);
			return ret;
		}
		accel_conf.odr = val->val1;
		ret = bma4_set_accel_config(&accel_conf, bma_dev);
		if (ret != BMA4_OK) {
			LOG_ERR("Failed to set Acceleration config err %d", ret);
			return ret;
		}
		break;

	case SENSOR_ATTR_FULL_SCALE:
		ret = bma4_get_accel_config(&accel_conf, bma_dev);
		if (ret != BMA4_OK) {
			LOG_ERR("Failed to get Acceleration config err %d", ret);
			return ret;
		}
		accel_conf.range = val->val1;
		ret = bma4_set_accel_config(&accel_conf, bma_dev);
		if (ret != BMA4_OK) {
			LOG_ERR("Failed to set Acceleration config err %d", ret);
			return ret;
		}
		break;

	default:
		break;
    }

	return 0;
}

static const struct sensor_driver_api bma421_driver_api = {
	.attr_set = bma421_attr_set,
#if CONFIG_BMA421_TRIGGER
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

	if (!device_is_ready(cfg->i2c.bus)) {
		LOG_ERR("I2C bus %s not ready", cfg->i2c.bus->name);
		return -ENODEV;
	}

	memset(bma_dev, 0, sizeof(struct bma4_dev));

	bma_dev->intf = BMA4_I2C_INTF;
	bma_dev->bus_read = user_i2c_read;
	bma_dev->bus_write = user_i2c_write;
	bma_dev->variant = BMA42X_VARIANT;
	bma_dev->intf_ptr = (void *)dev;
	bma_dev->delay_us = user_delay;
	bma_dev->read_write_len = 16;

	ret = bma4_soft_reset(bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("BMA4 soft reset error:%d", ret);
		return ret;
	}
	k_busy_wait(5000);

	ret = bma421_init(bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("BMA4 init error:%d", ret);
		return ret;
	}

	ret = bma421_write_config_file(bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("bma421_write_config_file failed err %d", ret);
		return ret;
	}

	ret = bma421_feature_enable(BMA421_STEP_CNTR, 1, bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("bma421_feature_enable failed err %d", ret);
		return ret;
	}

	ret = bma421_step_detector_enable(0, bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("bma421_step_detector_enable failed err %d", ret);
		return ret;
	}

	struct bma4_accel_config accel_conf = { 0 };
	accel_conf.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
	accel_conf.range = BMA4_ACCEL_RANGE_2G;
	accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
	accel_conf.perf_mode = BMA4_CIC_AVG_MODE;
	ret = bma4_set_accel_config(&accel_conf, bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("Failed to set Acceleration config err %d", ret);
		return ret;
	}

	ret = bma4_set_advance_power_save(BMA4_ENABLE, bma_dev);
	if (ret) {
		LOG_ERR("cannot activate power save state err %d", ret);
		return ret;
	}

	ret = bma4_set_accel_enable(BMA4_ENABLE, bma_dev);
	if (ret != BMA4_OK) {
		LOG_ERR("Accel enable failed err %d", ret);
		return ret;
	}
#ifdef CONFIG_BMA421_TRIGGER
	if (bma421_init_interrupt(dev) < 0) {
		LOG_DBG("Could not initialize interrupts");
		return -EIO;
	}
#endif
	LOG_INF("bma421 init done");
	return 0;
}

#define BMA421_INIT(index)   \
	static struct bma421_data bma421_data_##index; \
	static const struct bma421_config bma421_cfg_##index = { \
		.i2c = I2C_DT_SPEC_INST_GET(index), \
		COND_CODE_1(CONFIG_BMA421_TRIGGER, \
                (.int1_gpio = GPIO_DT_SPEC_INST_GET(index, int1_gpios)), ()) \
	}; \
	DEVICE_DT_INST_DEFINE(index, bma421_init_driver, NULL, \
				&bma421_data_##index, &bma421_cfg_##index, POST_KERNEL, \
				CONFIG_SENSOR_INIT_PRIORITY, &bma421_driver_api);

DT_INST_FOREACH_STATUS_OKAY(BMA421_INIT)
