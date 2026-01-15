/* SPDX-License-Identifier: Apache-2.0 */

#include "applib/event_service_client.h"
#include "services/common/event_service.h"
#include "system/logging.h"
#include "system/passert.h"

#include "kernel/events.h"
#include <zephyr/kernel.h>
#include <stdlib.h>
#include <string.h>

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
    return;
  }

  if (service->subscribers[subscription->task] == NULL) {
    // not subscribed
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
    // already subscribed
    return;
  }

  if (service->add_subscriber_callback != NULL) {
    service->add_subscriber_callback(subscription->task);
  }

  service->subscribers[subscription->task] = subscription->event_queue;
  ++service->num_subscribers;
}

static bool prv_event_service_send_event(struct k_msgq *queue, PebbleEvent *e) {
  if (queue == NULL) {
    return false;
  }

  int ret = k_msgq_put(queue, e, K_NO_WAIT);
  return (ret == 0);
}

void event_service_system_init(void) {
  // Initialize all event service entries to NULL
  for (int i = 0; i < PEBBLE_NUM_EVENTS; i++) {
    s_event_services[i] = NULL;
  }
}

void event_service_init(PebbleEventType type, EventServiceAddSubscriberCallback add_subscriber_callback,
    EventServiceRemoveSubscriberCallback remove_subscriber_callback) {
  if (type >= PEBBLE_NUM_EVENTS) {
    return;
  }

  if (s_event_services[type] == NULL) {
    // Allocate memory for the new event service
    s_event_services[type] = malloc(sizeof(EventServiceEntry));
    if (s_event_services[type] == NULL) {
      return;
    }

    // Initialize the new event service
    s_event_services[type]->num_subscribers = 0;
    for (int i = 0; i < NumPebbleTask; i++) {
      s_event_services[type]->subscribers[i] = NULL;
    }
  }

  // Update the callbacks
  s_event_services[type]->add_subscriber_callback = add_subscriber_callback;
  s_event_services[type]->remove_subscriber_callback = remove_subscriber_callback;
}

bool event_service_is_running(PebbleEventType event_type) {
  if (event_type >= PEBBLE_NUM_EVENTS || s_event_services[event_type] == NULL) {
    return false;
  }

  return (s_event_services[event_type]->num_subscribers > 0);
}

static bool prv_task_is_masked_out(PebbleEvent *e, PebbleTask task) {
  const PebbleTaskBitset task_bit = (1 << task);
  return (e->task_mask & task_bit);
}

void event_service_handle_event(PebbleEvent *e) {
  if (e == NULL) {
    return;
  }

  if (e->type >= PEBBLE_NUM_EVENTS) {
    return;
  }

  EventServiceEntry *service = s_event_services[e->type];
  if (service == NULL) {
    return;
  }

  // Send the event to all subscribed tasks
  for (int i = 0; i < NumPebbleTask; i++) {
    if (service->subscribers[i] != NULL && !prv_task_is_masked_out(e, (PebbleTask)i)) {
      // Create a copy of the event to send to each subscriber
      PebbleEvent event_copy;
      memcpy(&event_copy, e, sizeof(PebbleEvent));
      prv_event_service_send_event(service->subscribers[i], &event_copy);
    }
  }
}

void event_service_subscribe_from_kernel_main(PebbleSubscriptionEvent *subscription) {
  if (subscription == NULL) {
    return;
  }

  if (subscription->subscribe) {
    prv_event_service_subscribe(subscription);
  } else {
    prv_event_service_unsubscribe(subscription);
  }
}

void event_service_handle_subscription(PebbleSubscriptionEvent *subscription) {
  if (subscription == NULL) {
    return;
  }

  if (subscription->subscribe) {
    prv_event_service_subscribe(subscription);
  } else {
    prv_event_service_unsubscribe(subscription);
  }
}

void event_service_clear_process_subscriptions(PebbleTask task) {
  if (task >= NumPebbleTask) {
    return;
  }

  // Remove subscriptions for this task from all event services
  for (int i = 0; i < PEBBLE_NUM_EVENTS; i++) {
    EventServiceEntry *service = s_event_services[i];
    if (service != NULL && service->subscribers[task] != NULL) {
      PebbleSubscriptionEvent subscription = {
        .subscribe = false,
        .task = task,
        .event_type = (PebbleEventType)i,
        .event_queue = service->subscribers[task]
      };
      prv_event_service_unsubscribe(&subscription);
    }
  }
}

void* event_service_claim_buffer(PebbleEvent *e) {
  if (e == NULL) {
    return NULL;
  }

  void **buffer = event_get_buffer(e);
  if (buffer != NULL && *buffer != NULL) {
    // The buffer is already claimed or doesn't need to be claimed
    return *buffer;
  }

  return NULL;
}

void event_service_free_claimed_buffer(void *ref) {
  if (ref == NULL) {
    return;
  }

  // Free the buffer if it was allocated by the event service
  free(ref);
}