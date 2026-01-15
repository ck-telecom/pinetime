/* SPDX-License-Identifier: Apache-2.0 */

#include "applib/event_service_client.h"
#include "services/common/event_service.h"
#include "system/logging.h"
#include "system/passert.h"

#include "kernel/events.h"

typedef struct {
  int num_subscribers;
  struct k_msgq *subscribers[NumPebbleTask];
  EventServiceAddSubscriberCallback add_subscriber_callback;
  EventServiceRemoveSubscriberCallback remove_subscriber_callback;
} EventServiceEntry;

// There's an event service for each event so that
// System apps can also use the service
static EventServiceEntry *s_event_services[PEBBLE_NUM_EVENTS];

static void prv_event_service_unsubscribe(PebbleSubscriptionEvent *subscription) {
  EventServiceEntry *service = s_event_services[subscription->event_type];

  if (s_event_services[subscription->event_type] == NULL) {
    // service does not exist
    //PBL_LOG(LOG_LEVEL_WARNING, "Attempted to unsubscribe from %d, no service found",
    //    subscription->event_type);
    return;
  }

  if (service->subscribers[subscription->task] == NULL) {
    // not subscribed
    //PBL_LOG(LOG_LEVEL_WARNING, "Attempted to unsubscribe from %d, not subscribed",
    //    subscription->event_type);
    return;
  }

  PBL_ASSERTN(service->num_subscribers > 0);
  --service->num_subscribers;
  service->subscribers[subscription->task] = NULL;
  if (service->remove_subscriber_callback != NULL) {
    service->remove_subscriber_callback(subscription->task);
  }
}

static void prv_event_service_subscribe(PebbleSubscriptionEvent *subscription) {
  EventServiceEntry *service = s_event_services[subscription->event_type];

  if (service == NULL) {
    // No event service for this event type, create one
    event_service_init(subscription->event_type, NULL, NULL);
    service = s_event_services[subscription->event_type];
  }

  if (service->subscribers[subscription->task]) {
    // already subscribed ?
    //PBL_LOG(LOG_LEVEL_DEBUG, "already subscribed");
    return;
  }

  if (service->add_subscriber_callback != NULL) {
    service->add_subscriber_callback(subscription->task);
  }

  service->subscribers[subscription->task] = subscription->event_queue;
  ++service->num_subscribers;
}

static bool prv_event_service_send_event(struct k_msgq *queue, PebbleEvent *e) {
}

void event_service_system_init(void) {

}

void event_service_init(PebbleEventType type, EventServiceAddSubscriberCallback add_subscriber_callback,
    EventServiceRemoveSubscriberCallback remove_subscriber_callback) {

  s_event_services[type]->add_subscriber_callback = add_subscriber_callback;
  s_event_services[type]->remove_subscriber_callback = remove_subscriber_callback;
}

bool event_service_is_running(PebbleEventType event_type) {
  if (s_event_services[event_type] == NULL) {
    return (false);
  }
  if (s_event_services[event_type]->num_subscribers > 0) {
    return (true);
  }

  return (false);
}

static bool prv_task_is_masked_out(PebbleEvent *e, PebbleTask task) {
  const PebbleTaskBitset task_bit = (1 << task);
  return (e->task_mask & task_bit);
}

void event_service_handle_event(PebbleEvent *e) {
  EventServiceEntry *service = s_event_services[e->type];
  if (service == NULL) {
    return;
  }

}

void event_service_subscribe_from_kernel_main(PebbleSubscriptionEvent *subscription) {

}

void event_service_handle_subscription(PebbleSubscriptionEvent *subscription) {

}

void event_service_clear_process_subscriptions(PebbleTask task) {

}

void* event_service_claim_buffer(PebbleEvent *e) {

}

void event_service_free_claimed_buffer(void *ref) {

}
