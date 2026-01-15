#include "applib/event_service_client.h"
#include <zephyr/kernel.h>
#include <string.h>

static sys_slist_t g_event_handlers = SYS_SLIST_STATIC_INIT(&g_event_handlers);

void event_service_client_subscribe(EventServiceInfo * service_info) {
  if (service_info == NULL || service_info->handler == NULL) {
    return;
  }

  // Add the service info to the list of event handlers
  sys_slist_append(&g_event_handlers, &service_info->list_node);
}

void event_service_client_unsubscribe(EventServiceInfo * service_info) {
  if (service_info == NULL) {
    return;
  }

  // Remove the service info from the list of event handlers
  sys_slist_find_and_remove(&g_event_handlers, &service_info->list_node);
}

void event_service_client_handle_event(PebbleEvent *e) {
  if (e == NULL) {
    return;
  }

  sys_snode_t *node;

  // Iterate through all event handlers and call the ones that match the event type
  SYS_SLIST_FOR_EACH_NODE(&g_event_handlers, node) {
    EventServiceInfo *service_info = CONTAINER_OF(node, EventServiceInfo, list_node);
    if (service_info->type == e->type) {
      service_info->handler(e, service_info->context);
    }
  }
}

bool event_service_filter(sys_snode_t *node, void *tp) {
  if (node == NULL || tp == NULL) {
    return false;
  }

  EventServiceInfo *service_info = CONTAINER_OF(node, EventServiceInfo, list_node);
  PebbleEventType *event_type = (PebbleEventType *)tp;

  return (service_info->type == *event_type);
}

