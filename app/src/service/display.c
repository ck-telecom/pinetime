/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <logging/log.h>

#include "display.h"
#include "../view.h"
//#include "../event/event_service.h"

#define DISPLAY_STACK_SIZE      1024
#define DISPLAY_PRIORITY        5

K_MSGQ_DEFINE(event_msgq, sizeof(struct msg), 10, 4);

LOG_MODULE_REGISTER(display, LOG_LEVEL_INF);

int msg_send_event(struct msg *m, unsigned long type, unsigned long event)
{
    m->type = type;
    m->event = event;
    return k_msgq_put(&event_msgq, m, K_NO_WAIT);
}

int msg_send_gesture(struct msg *m, uint32_t gesture)
{
    m->type = MSG_TYPE_GESTURE;
    m->gesture = gesture;
    return k_msgq_put(&event_msgq, m, K_NO_WAIT);
}

int msg_send_button(struct msg *m, uint32_t btn_event)
{
    m->type = MSG_TYPE_BUTTON;
    m->event = btn_event;
    return k_msgq_put(&event_msgq, m, K_NO_WAIT);
}

int msg_send_data(struct msg *m, uint32_t type, void *data)
{
    m->type = type;
    m->data = data;
    return k_msgq_put(&event_msgq, m, K_NO_WAIT);
}

static struct view *current_screen;

void display_event_handler(struct msg *m)
{
    if (current_screen->event)
        current_screen->event(current_screen, m);
}

void display_thread(void* arg1, void *arg2, void *arg3)
{
    struct msg m;
    int ret;
    const struct device *display_dev;
    k_timeout_t timeout = K_MSEC(10);

    display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
    if (display_dev == NULL) {
        LOG_ERR("device not found.  Aborting test.");
        return;
    }
    display_blanking_off(display_dev);

current_screen = &home;
view_init(current_screen, lv_scr_act());
    while (1) {
        ret = k_msgq_get(&event_msgq, &m, timeout);
        display_event_handler(&m);
/*        if (ret == 0) {
            if (m.type == MSG_TYPE_GESTURE) {
                LOG_INF("gesture:%d", m.gesture);

            }
        }

        switch (m.type) {
        case MSG_TYPE_GESTURE:
            display_gesure_handler(m.gesture);
            break;

        case MSG_TYPE_EVT:
            event_service_event_trigger(m.command, m.context, NULL);
            break;

        case MSG_SLEEP:
            backlight_enable(false);
            display_power_sleep();
            timeout = K_FOREVER;
            break;

        case MSG_WAKEUP:
            display_power_wakeup();
            backlight_enable(true);
            timeout = K_MSEC(10);
            break;

        case MSG_TYPE_BUTTON:
            break;

        case MSG_BLE_CONNECTION:
            break;

        case MSG_TYPE_CHARGER:
            break;

        default:
            break;
        }
*/
        lv_task_handler();
        LOG_DBG("lv_task_handler");
    }
}

K_THREAD_DEFINE(dispaly, DISPLAY_STACK_SIZE, display_thread, NULL, NULL, NULL, DISPLAY_PRIORITY, 0, 0);
