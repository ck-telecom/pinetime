#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <logging/log.h>


#define BMA4XX_STACK_SIZE   1024
#define BMA4XX_PRIORITY     5

LOG_MODULE_REGISTER(bma4xx, LOG_LEVEL_INF);

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

    }
}

K_THREAD_DEFINE(bma4xx_tid, BMA4XX_STACK_SIZE, bma4xx_thread, NULL, NULL, NULL, BMA4XX_PRIORITY, 0, 0);