/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _EVT_H
#define _EVT_H

#define EVT_CLOCK_TICK          (1 << 0)
#define EVT_BATTERY_STATUS      (1 << 1)
#define EVT_CHARGER_STATUS
#define EVT_COMPASS_CHANGED
#define EVT_BUTTON_PRESSED      (1 << 5)
#define EVT_BUTTON_RELEASE      (1 << 6)

#include <sys/slist.h>
/*
struct evt_node {
    sys_node_t node;
    uint32_t event;
    struct view *view;
};

void evt_add_handler(struct evt *ctx, struct evt_node *node);
void evt_del_handler(evt_node *node);
*/
void event_trigger(uint32_t event);

#endif

