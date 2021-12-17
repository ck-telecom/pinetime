#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <logging/log.h>
#include <lvgl.h>
#include "cst816s.h"
#include "display_api.h"

#define TOUCH_STACK_SIZE    1024
#define TOUCH_PRIORITY      5

#define TOUCH_DEV "CST816S"

LOG_MODULE_REGISTER(touch, LOG_LEVEL_INF);

K_MSGQ_DEFINE(kscan_msgq, sizeof(lv_indev_data_t),
          16, 4);

static bool lvgl_indev_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
#if 0
    lv_indev_data_t curr;

    static lv_indev_data_t prev = {
        .point.x = 0,
        .point.y = 0,
        .state = LV_INDEV_STATE_REL,
    };

    prev.state = LV_INDEV_STATE_REL;
    if (k_msgq_get(&kscan_msgq, &curr, K_NO_WAIT) != 0) {
        goto set_and_release;
    }

    prev = curr;

set_and_release:
    *data = prev;

    return k_msgq_num_used_get(&kscan_msgq) > 0;
#else
    const struct device *dev = drv->user_data;
    if (sensor_sample_fetch(dev) < 0) {
        LOG_ERR("Touch sample update error.");
    }

    struct sensor_value state;
    struct sensor_value touch_point;

    sensor_channel_get(dev, CST816S_CHAN_TOUCH_POINT_1, &touch_point);

    sensor_channel_get(dev, CST816S_CHAN_STATE, &state);

    data->point.x = touch_point.val1;
    data->point.y = touch_point.val2;
    data->state = (state.val1 == UP) ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;

    return false;
#endif
}

static int lvgl_indev_init(void *dev)
{
    lv_indev_drv_t indev_drv;

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_indev_read_cb;
    indev_drv.user_data = (void *)dev;

    if (lv_indev_drv_register(&indev_drv) == NULL) {
        LOG_ERR("Failed to register input device.");
        return -EPERM;
    }

    return 0;
}

void touch_isr_handler(const struct device *touch_dev, struct sensor_trigger *tap)
{
    enum cst816s_gesture gesture;
    struct sensor_value touch_point;

    if (sensor_sample_fetch(touch_dev) < 0) {
        LOG_ERR("Touch sample update error.");
    }

//    display_wake_up();
//    backlight_enable(true);
//    k_timer_start(&display_off_timer, DISPLAY_TIMEOUT, K_NO_WAIT);

    struct sensor_value gesture_val;
    sensor_channel_get(touch_dev, CST816S_CHAN_GESTURE, &gesture_val);
    gesture = gesture_val.val1;
    sensor_channel_get(touch_dev, CST816S_CHAN_TOUCH_POINT_1, &touch_point);

    if (gesture == CLICK) {/*
        lv_indev_data_t data = {
            .point.x = touch_point.val1,
            .point.y = touch_point.val2,
            .state = LV_INDEV_STATE_PR,
        };
        LOG_INF("Gesture %d on x=%d, y=%d", gesture, touch_point.val1, touch_point.val2);
        if (k_msgq_put(&kscan_msgq, &data, K_NO_WAIT) != 0) {
            LOG_ERR("Could put input data into queue");
        }*/
    } else {
        msg_send_gesture(gesture);
        LOG_DBG("Gesture %d", gesture);
    }
}

int touch_init(const struct device *dev)
{
    const struct device *touch_dev = device_get_binding(TOUCH_DEV);
    if (!touch_dev) {
        LOG_ERR("could get %s device", TOUCH_DEV);
        return -ENODEV;
    }
#if 0
    if (IS_ENABLED(CONFIG_CST816S_TRIGGER)) {
        struct sensor_trigger trig = {
            .type = SENSOR_TRIG_DATA_READY,
            .chan = SENSOR_CHAN_ACCEL_XYZ,
        };
        if (sensor_trigger_set(touch_dev, &trig, touch_isr_handler) < 0) {
            LOG_ERR("Cannot configure trigger");
            return -ENOTSUP;
        }
    }
#endif
    return lvgl_indev_init((void *)touch_dev);
}
SYS_INIT(touch_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void touch_thread()
{
    while (1) {
        k_sleep(K_MSEC(100));
    }
}
K_THREAD_DEFINE(touch_tid, TOUCH_STACK_SIZE, touch_thread, NULL, NULL, NULL, TOUCH_PRIORITY, 0, 0);