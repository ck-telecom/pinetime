/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "view.h"

#define GUI_STACK_SIZE 1024
#define GUI_PRIORITY 5

struct gui_msg {
    uint32_t type;
    union {
        void *ptr;          /**< for MSG_TYPE_GUI */
        uint32_t event;     /**< for MSG_TYPE_EVENT */
        uint32_t gesture;   /**< for MSG_TYPE_GESTURE */
    };
};

struct gui gui_ctx {
    struct view *active_widget;
};

K_MSGQ_DEFINE(c_msgq, sizeof(struct msg), 10, 4);

void switch_ui(struct gui *ctx, struct widget *w)
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
        switch_ui(ctx, gm->data);
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
        if (v->update_draw) {
            w->update_draw(w);//redraw later
            w->flags &= ~WF_DIRTY;
        }
        return 1;
    }
    return 0;
}

ctrl_handler()
{

}

void gesure_handler(struct gui *ctx, uint32_t gesture)
{
    struct widget *w = ctx->active_widget;
    if (w && w->gui_event)
        w->gui_event(w, gesture);
}

void gui_thread(void* arg1, void *arg2, void *arg3)
{
    struct gui_msg m;
    struct gui *ctx;

    while (1)
    {
        k_msgq_get(&c_msgq, &m, K_MSEC(10)/* K_FOREVER */); //used timeout to refresh UI
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

K_THREAD_DEFINE(gui_tid, GUI_STACK_SIZE, gui_thread, NULL, NULL, NULL, GUI_PRIORTY, 0, 0);
