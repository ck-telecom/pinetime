/*
 * Copyright (c) 2021 gouqs@hotmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr.h>
#include <logging/log.h>
#include <kernel.h>

#include "event_service.h"

#define EVENT_STACK_SIZE 1024
#define EVENT_PRIORITY 5

LOG_MODULE_REGISTER(event_service, LOG_LEVEL_INF);

K_MSGQ_DEFINE(eventq, sizeof(struct event), 10, 4);

int event_service_post(struct event *event)
{
    return k_msgq_put(&eventq, event, K_NO_WAIT);
}

void event_service_dispatch(struct event *event)
{
    if (event->handler)
        event->handler(event);
}

void event_service_thread(void* arg1, void *arg2, void *arg3)
{
    struct event event;
    while (1)
    {
        k_msgq_get(&eventq, &event, K_FOREVER);
        event_service_dispatch(&event);
    }
}

K_THREAD_DEFINE(event_tid, EVENT_STACK_SIZE, event_service_thread, NULL, NULL, NULL, EVENT_PRIORITY, 0, 0);
