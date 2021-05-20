/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _EVT_H
#define _EVT_H

#define EVT_CLOCK_TICK
#define EVT_BATTERY_STATUS
#define EVT_CHARGER_STATUS
#define EVT_COMPASS_CHANGED
#define EVT_

#include <sys/slist.h>

struct evt_node {
    sys_node_t node;
    uint32_t event;
    struct view *view;
};

void evt_add_handler(struct evt *ctx, struct evt_node *node);
void evt_del_handler(evt_node *node);

#endif

