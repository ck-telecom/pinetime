#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <logging/log.h>
#include <math.h>

#include "bma4.h"

#define BMA4XX_STACK_SIZE   1024
#define BMA4XX_PRIORITY     5

LOG_MODULE_REGISTER(bma4xx, LOG_LEVEL_INF);

#ifdef CONFIG_BMA421_TRIGGER
static void bma421_handler(const struct device *dev,
			struct sensor_trigger *trig)
{
    struct sensor_value val[3];

    if (sensor_sample_fetch(dev) < 0) {
        LOG_ERR("sensor_sample_fetch error");
        return;
    }

	if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, val) < 0) {
        LOG_ERR("sensor_channel_get error");
        return;
    }
    LOG_INF("bma421 handler");
}
#endif

static void bma4xx_thread()
{
    const struct device *dev = device_get_binding("BMA421");
    if (dev == NULL) {
        LOG_ERR("could not get BMA421 device");
        return;
    }

    if (IS_ENABLED(CONFIG_BMA421_TRIGGER)) {
        struct sensor_trigger trig = {
            .type = SENSOR_TRIG_DATA_READY,
            .chan = SENSOR_CHAN_ALL,
        };
        if (sensor_trigger_set(dev, &trig, bma421_handler) < 0) {
            LOG_ERR("Cannot configure trigger");
            return;
        }
    }

    while (1) {
        struct sensor_value accel_data[3] = { 0 };
        struct sensor_value ori[3] = { 0 };
        double orientation[3] = { 0 };

        if (sensor_sample_fetch(dev) < 0) {
            LOG_ERR("sensor_sample_fetch error");
        }
        if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel_data) < 0) {
            LOG_ERR("sensor_channel_get error");
        }
        double x = sensor_value_to_double(&accel_data[0]);
        double y = sensor_value_to_double(&accel_data[1]);
        double z = sensor_value_to_double(&accel_data[2]);
/*
        LOG_INF("sensor_channel_get accel.X %d.%d", accel_data[0].val1, accel_data[0].val2);
        LOG_INF("sensor_channel_get accel.Y %d.%d", accel_data[1].val1, accel_data[1].val2);
        LOG_INF("sensor_channel_get accel.Z %d.%d", accel_data[2].val1, accel_data[2].val2);
*/
        orientation[0] = (float) atan2(x, sqrt(y * y + z * z));
        orientation[1] = (float) atan2(y, sqrt(x * x + z * z));
        orientation[2] = (float) atan2(x, sqrt(y * y + x * x));

        sensor_value_from_double(&ori[0], orientation[0]);
        sensor_value_from_double(&ori[1], orientation[1]);
        sensor_value_from_double(&ori[2], orientation[2]);

        int32_t pitch = sensor_rad_to_degrees(&ori[0]);
        int32_t roll  = sensor_rad_to_degrees(&ori[1]);
        int32_t yaw   = sensor_rad_to_degrees(&ori[2]);

        LOG_DBG("pitch %d, roll %d, yaw %d", pitch, roll, yaw);

        k_sleep(K_MSEC(500));
    }
}

K_THREAD_DEFINE(bma4xx_tid, BMA4XX_STACK_SIZE, bma4xx_thread, NULL, NULL, NULL, BMA4XX_PRIORITY, 0, 0);