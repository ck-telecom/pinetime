/*
 * Copyright (c) 2021 gouqs@hotmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include "event.h"

#include <sys/slist.h>

#define EVENT_STACK_SIZE 1024
#define EVENT_PRIORITY 5

static struct k_poll_signal signal;
#if 0
struct evt_node {
    uint32_t type;
    uint32_t id;
    void *ptr;
    struct sys_slist_t evt_list;
};

static sys_slist_t evt_list;



struct evt evt_ctx {
    struct widget *active_widget;
};




void evt_add_handler(struct evt *ctx, struct evt_node *node)
{
    sys_slist_append(&ctx->evt_list, node);
}

void evt_del_handler(evt_node *node)
{
    struct evt_node *n;
    SYS_SLIST_FOR_EACH_NODE(evt_list, n) {
        if (n == node)
            sys_slist_find_and_remove();
    }
}

int evt_submit(evt_t evt)
{
    struct evt_node *n;
    SYS_SLIST_FOR_EACH_NODE(evt_list, n) {
        if (n->event & evt)
            n->event_cb(evt);
    }
}

void evt_handler(struct gui *ctx, struct msg *m)
{
    struct evt_node *n;
    SYS_SLIST_FOR_EACH_NODE(&ctx->evt_list, n) {
        if (n->view.event && n->event & EVT_COMPASS_CHANGED)
            n->view.event(n->view, event);
    }
}
#endif
void event_trigger(uint32_t event)
{
    k_poll_signal_raise(&signal, event);
}

void event_thread(void* arg1, void *arg2, void *arg3)
{
    k_poll_signal_init(&signal);

    struct k_poll_event events[1] = {
        K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL,
                                 K_POLL_MODE_NOTIFY_ONLY,
                                 &signal),
    };

    while (1)
    {
        k_poll(events, 1, K_FOREVER);

        if (events[0].signal->result == EVT_CLOCK_TICK) {
//            evt_submit(EVT_CLOCK_TICK);
        } else if (events[0].signal->result == EVT_BATTERY_STATUS) {
//            evt_submit(EVT_BATTERY_STATUS);
        } else if (events[0].signal->result == EVT_BUTTON_PRESSED) {
            //???
        } else if (events[0].signal->result == EVT_BUTTON_RELEASE) {

        }


        events[0].signal->signaled = 0;
        events[0].state = K_POLL_STATE_NOT_READY;
    }
}

K_THREAD_DEFINE(event_tid, EVENT_STACK_SIZE, event_thread, NULL, NULL, NULL, EVENT_PRIORITY, 0, 0);
