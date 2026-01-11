/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "applib/event_service_client.h"

#include "system/logging.h"
#include "system/passert.h"
#include <zephyr/sys/slist.h>

static sys_slist_t event_handlers_list = SYS_SLIST_STATIC_INIT(&event_handlers_list);

static int event_service_comparator(EventServiceInfo *a, EventServiceInfo *b) {
  return (b->type - a->type);
}

static void do_handle(EventServiceInfo *info, PebbleEvent *e) {
  if (info->handler) {
    info->handler(e, info->context);
  }
}

void event_service_client_subscribe(EventServiceInfo *service_info) {
  if (!service_info) {
    return;
  }

  // Add the service info to the handlers list
  sys_slist_append(&event_handlers_list, &service_info->list_node);
}

void event_service_client_unsubscribe(EventServiceInfo *service_info) {
  if (!service_info) {
    return;
  }

  // Remove the service info from the handlers list
  sys_slist_find_and_remove(&event_handlers_list, &service_info->list_node);
}

void event_service_client_handle_event(PebbleEvent *e) {
  if (!e) {
    return;
  }

  // Iterate through all handlers and call those matching the event type
  struct sys_snode *node;
  sys_slist_for_each_node(&event_handlers_list, node) {
    EventServiceInfo *info = CONTAINER_OF(node, EventServiceInfo, list_node);
    if (info->type == e->type) {
      do_handle(info, e);
    }
  }
}

bool event_service_filter(sys_snode_t *node, void *tp) {
  if (!node || !tp) {
    return false;
  }

  EventServiceInfo *info = CONTAINER_OF(node, EventServiceInfo, list_node);
  PebbleEventType *event_type = (PebbleEventType *)tp;

  return (info->type == *event_type);
}
