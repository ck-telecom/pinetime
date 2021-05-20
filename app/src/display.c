/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "view.h"

#include "backlight.h"

#define GUI_STACK_SIZE 1024
#define GUI_PRIORITY 5



struct gui gui_ctx {
    struct view *active_widget;
};

K_MSGQ_DEFINE(gui_msgq, sizeof(struct msg), 10, 4);

int msg_send_event(struct msg *m, uint32_t event)
{
    m->type = MSG_TYPE_EVT;
    m->event = event;
    return k_msgq_put(&gui_msgq, m, K_NO_WAIT);
}

int msg_send_data(struct msg *m, uint32_t type, void *data)
{
    m->type = type;
    m->data = data;
    return k_msgq_put(&gui_msgq, m, K_NO_WAIT);
}

int msg_send_gesture(struct msg *m, uint32_t gesture)
{
    m->type = MSG_TYPE_GESTURE;
    m->gesture = gesture;
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

void gesure_handler(struct gui *ctx, uint32_t gesture)
{
    struct widget *w = ctx->active_widget;
    if (w && w->gui_event)
        w->gui_event(w, gesture);
}

void display_thread(void* arg1, void *arg2, void *arg3)
{
    struct msg m;
    struct gui *ctx;

    while (1)
    {
        k_msgq_get(&gui_msgq, &m, K_MSEC(10)/* K_FOREVER */); //used timeout to refresh UI
        if (m.type == MSG_TYPE_GUI) {
            gui_handler(ctx, &m);
        } else if (m.type == MSG_TYPE_EVT) {
            evt_handler(ctx, m.event);
        } else if(m.type == MSG_TYPE_GESTURE) {
            gesture_handler(ctx, m.gesture);
        }

        // view_update_draw(ctx->active_widget);
    }
}

K_THREAD_DEFINE(gui_tid, GUI_STACK_SIZE, display_thread, NULL, NULL, NULL, GUI_PRIORTY, 0, 0);
