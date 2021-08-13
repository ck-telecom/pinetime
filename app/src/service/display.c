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

#define DISPLAY_STACK_SIZE      1024
#define DISPLAY_PRIORITY        5

K_MSGQ_DEFINE(event_msgq, sizeof(struct msg), 10, 4);

LOG_MODULE_REGISTER(display, LOG_LEVEL_INF);

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

static struct view *current_screen;
#if 0
struct gui gui_ctx {
    struct view *active_widget;
};





int msg_send_data(struct msg *m, uint32_t type, void *data)
{
    m->type = type;
    m->data = data;
    return k_msgq_put(&gui_msgq, m, K_NO_WAIT);
}



void widget_active(struct gui *ctx, struct widget *w)
{
    if (ctx->active_widget == w) {
        return;
    }
    if (gui->active_widget)
        view_close(w);

    view_draw(w);
    lv_scr_load(view_container(w));
//  lv_scr_load_anim(view_container(w), LV_SCR_LOAD_ANIM_MOVE_LEFT, 1000, 0, NULL);
// lv_disp_set_direction
    ctx->active_widget = w;
}

void gui_handler(struct gui *ctx, struct msg *m)
{
    struct gui_msg *gm = (struct gui_msg *)m->ptr;
    switch (gm->id) {
    case MSG_ID_GUI_SWITCH :
    case MSG_ID_GUI_SWITCH_UP :
        widget_active(ctx, gm->data);
        break;
    default:
        break;
    }
}

int evt_handler(struct gui *ctx, uint32_t evt)
{
    struct widget *w = ctx->active_widget;
    if (w == NULL)
        return;

    if ( (w->flags & WF_ACTIVE) && (w->event) && event ) {
        w->event(w, evt);
    }
    if ( (w->flags & WF_VISIBLE) && (w->flags & WF_DIRTY) ) {
        if (v->redraw) {
            w->redraw(w);//redraw later
            w->flags &= ~WF_DIRTY;
        }
        return 1;
    }
    return 0;
}


#endif

void display_gesure_handler(uint32_t gesture)
{
    if (current_screen->event)
        current_screen->event(current_screen, gesture);
}

extern lv_obj_t *screen_home_draw();
extern lv_obj_t *clock_face_create(lv_obj_t *p);
void display_thread(void* arg1, void *arg2, void *arg3)
{
    struct msg m;
    int ret;
    const struct device *display_dev;
    display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
    if (display_dev == NULL) {
        LOG_ERR("device not found.  Aborting test.");
        return;
    }
    display_blanking_off(display_dev);

current_screen = &home;
view_init(current_screen, lv_scr_act());
    while (1)
    {
        ret = k_msgq_get(&event_msgq, &m, K_MSEC(10)); // 10 ms timeout
        if (ret == 0) {
            if (m.type == MSG_TYPE_GESTURE) {
                LOG_INF("gesture:%d", m.gesture);
                display_gesure_handler(m.gesture);
            }
        }
/*       if (m.type == MSG_TYPE_GUI) {
            gui_handler(ctx, &m);
        } else if (m.type == MSG_TYPE_EVT) {
            evt_handler(ctx, m.event);
        } else if(m.type == MSG_TYPE_GESTURE) {
            gesture_handler(ctx, m.gesture);
        }*/
        lv_task_handler();
        // LOG_INF("lv_task_handler");
    }
}

K_THREAD_DEFINE(dispaly, DISPLAY_STACK_SIZE, display_thread, NULL, NULL, NULL, DISPLAY_PRIORITY, 0, 0);
