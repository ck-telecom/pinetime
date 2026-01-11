/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#pragma once

#include "kernel/events.h"
#include <zephyr/sys/slist.h>

typedef void (*EventServiceEventHandler)(PebbleEvent *e, void *context);

typedef struct __PACKED {
  sys_snode_t list_node;
  PebbleEventType type;
  EventServiceEventHandler handler;
  void *context;
} EventServiceInfo;

void event_service_client_subscribe(EventServiceInfo * service_info);
void event_service_client_unsubscribe(EventServiceInfo * service_info);
void event_service_client_handle_event(PebbleEvent *e);
bool event_service_filter(sys_snode_t *node, void *tp);
