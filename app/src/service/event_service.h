/*
 * Copyright (c) 2021 gouqs@hotmail.com.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _EVENT_SERVICE_H
#define _EVENT_SERVICE_H

struct event;

typedef void (*handler)(struct event *event);

struct event {
    uint32_t type;
    handler handler;
};

int event_service_post(struct event *event);

#endif /* _EVENT_SERVICE_H */
