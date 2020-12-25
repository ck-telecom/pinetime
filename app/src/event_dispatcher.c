/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "view.h"

#include <sys/slist.h>

#define EVENT_STACK_SIZE 1024
#define EVENT_PRIORITY 5

struct evt {
    uint32_t type;
    uint32_t id;
    void *ptr;
    struct sys_slist_t evt_list;
};

static sys_slist_t evt_list;



struct evt evt_ctx {
    struct widget *active_widget;
};

K_MSGQ_DEFINE(evt_msgq, sizeof(struct msg), 10, 4);


void evt_add_handler(struct evt *ctx, struct evt_node *node)
{
    sys_slist_append(&ctx->evt_list, node);
}

void evt_del_handler(evt_node *node)
{
    
}

void evt_handler(struct gui *ctx, struct msg *m)
{
    struct evt_node *n;
    SYS_SLIST_FOR_EACH_NODE(&ctx->evt_list, n) {
        if (n->view.event && n->event & EVT_COMPASS_CHANGED)
            n->view.event(n->view, event);
    }
}

void event_thread(void* arg1, void *arg2, void *arg3)
{
    struct evt m;
    struct evt *ctx;

    sys_slist_init(&evt_list);

    while (1)
    {
        k_msgq_get(&evt_msgq, &m, K_MSEC(10)/* K_FOREVER */);
        if (m.type == MSG_TYPE_GUI) {
            gui_handler(ctx, &m);
        } else if (m.type == MSG_TYPE_EVT) {
            evt_handler(ctx, &m);
        } else if(m.type == MSG_TYPE_GESTURE) {
            // gesture_handler();
        }

        view_update_draw(ctx->active_widget);
    }
}

K_THREAD_DEFINE(event_tid, EVENT_STACK_SIZE, event_thread, NULL, NULL, NULL, EVENT_PRIORITY, 0, 0);
