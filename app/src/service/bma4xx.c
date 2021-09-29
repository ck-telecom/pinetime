#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "bma4.h"

#define BMA4XX_STACK_SIZE   1024
#define BMA4XX_PRIORITY     5

LOG_MODULE_REGISTER(bma4xx, LOG_LEVEL_INF);

extern struct bma4_dev bma;

static void bma421_handler(const struct device *dev,
			   struct sensor_trigger *trig)
{
	//process_sample(dev);
    LOG_INF("bma421 handler");
}

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
        struct bma4_accel data;
        bma4_read_accel_xyz(&data, &dev);

  //uint32_t steps = 0;
 // bma4_step_counter_output(&steps, &bma);

  //int32_t temperature;
  //bma4_get_temperature(&temperature, BMA4_DEG, &bma);
  //temperature = temperature / 1000;

  //uint8_t activity = 0;
  //bma423_activity_output(&activity, &bma);

  LOG_INF("%d/%d/%d", data.x, data.y, data.z);
        k_sleep(K_MSEC(500));
    }
}

K_THREAD_DEFINE(bma4xx_tid, BMA4XX_STACK_SIZE, bma4xx_thread, NULL, NULL, NULL, BMA4XX_PRIORITY, 0, 0);