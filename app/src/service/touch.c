#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "cst816s.h"

#define TOUCH_STACK_SIZE    1024
#define TOUCH_PRIORITY      5

#define TOUCH_DEV "CST816S"

LOG_MODULE_REGISTER(touch, LOG_LEVEL_INF);

void touch_isr_handler(const struct device *touch_dev, struct sensor_trigger *tap)
{
    enum cst816s_gesture gesture;
    struct sensor_value touch_point;

    if (sensor_sample_fetch(touch_dev) < 0) {
        LOG_ERR("Touch sample update error.");
    }

//	display_wake_up();
//	backlight_enable(true);
//	k_timer_start(&display_off_timer, DISPLAY_TIMEOUT, K_NO_WAIT);

    struct sensor_value gesture_val;
    sensor_channel_get(touch_dev, CST816S_CHAN_GESTURE, &gesture_val);
    gesture = gesture_val.val1;
    sensor_channel_get(touch_dev, CST816S_CHAN_TOUCH_POINT_1, &touch_point);

    LOG_INF("Gesture %d on x=%d, y=%d", gesture, touch_point.val1, touch_point.val2);
	// gui_handle_touch_event(touch_dev, gesture);
}

int touch_init(const struct device *dev)
{
    const struct device *touch_dev = device_get_binding(TOUCH_DEV);
    if (!touch_dev) {
        LOG_ERR("could get %s device", TOUCH_DEV);
        return;
    }
    if (IS_ENABLED(CONFIG_CST816S_TRIGGER)) {
        struct sensor_trigger trig = {
            .type = SENSOR_TRIG_DATA_READY,
            .chan = SENSOR_CHAN_ACCEL_XYZ,
        };
        if (sensor_trigger_set(touch_dev, &trig, touch_isr_handler) < 0) {
            LOG_ERR("Cannot configure trigger");
            return;
        }
    }
    return 0;
}
SYS_INIT(touch_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

static void touch_thread()
{
    while (1) {
        k_sleep(K_MSEC(100));
    }
}
K_THREAD_DEFINE(touch_tid, TOUCH_STACK_SIZE, touch_thread, NULL, NULL, NULL, TOUCH_PRIORITY, 0, 0);