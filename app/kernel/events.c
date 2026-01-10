/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "events.h"

#include "debug/setup.h"

#include "system/logging.h"
#include "system/passert.h"
#include "system/reset.h"

#include "kernel/pbl_malloc.h"
#include "os/tick.h"

#include "services/normal/app_outbox_service.h"
#include "syscall/syscall.h"

#include <zephyr/kernel.h>
#include <freertos_types.h>
#include <portmacro.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Zephyr message queues for events
static struct k_msgq *s_kernel_event_queue = NULL;
static struct k_msgq *s_from_app_event_queue = NULL;
static struct k_msgq *s_from_worker_event_queue = NULL;
static struct k_msgq *s_from_kernel_event_queue = NULL;

// Poll events for waiting on multiple queues
static struct k_poll_event poll_events[3];

static const int MAX_KERNEL_EVENTS = 32;
static const int MAX_FROM_APP_EVENTS = 10;
static const int MAX_FROM_WORKER_EVENTS = 5;
static const int MAX_FROM_KERNEL_MAIN_EVENTS = 14;

// Event queue buffers (statically allocated)
static PebbleEvent s_kernel_event_buffer[MAX_KERNEL_EVENTS];
static PebbleEvent s_from_app_event_buffer[MAX_FROM_APP_EVENTS];
static PebbleEvent s_from_worker_event_buffer[MAX_FROM_WORKER_EVENTS];
static PebbleEvent s_from_kernel_event_buffer[MAX_FROM_KERNEL_MAIN_EVENTS];

uint32_t s_current_event;

#define EVENT_DEBUG 0

#if EVENT_DEBUG
static void prv_queue_dump(QueueHandle_t queue) {
  PebbleEvent event;
  PBL_LOG(LOG_LEVEL_DEBUG, "Dumping queue:");
  while (xQueueReceive(queue, &event, 0) == pdTRUE) {
    PBL_LOG(LOG_LEVEL_DEBUG, "Event type: %u", event.type);
  }
  for(;;);
}
#endif

void events_init(void) {
  // This assert is to make sure we don't accidentally bloat our PebbleEvent unecessarily. If you hit this
  // assert and you have a good reason for making the event bigger, feel free to relax the restriction.
  //PBL_LOG(LOG_LEVEL_DEBUG, "PebbleEvent size is %u", sizeof(PebbleEvent));
  // FIXME:
  _Static_assert(sizeof(PebbleEvent) <= 12,
                 "You made the PebbleEvent bigger! It should be no more than 12");

  // Create and initialize Zephyr message queues
  s_kernel_event_queue = k_malloc(sizeof(struct k_msgq));
  s_from_app_event_queue = k_malloc(sizeof(struct k_msgq));
  s_from_worker_event_queue = k_malloc(sizeof(struct k_msgq));
  s_from_kernel_event_queue = k_malloc(sizeof(struct k_msgq));

  PBL_ASSERTN(s_kernel_event_queue != NULL);
  PBL_ASSERTN(s_from_app_event_queue != NULL);
  PBL_ASSERTN(s_from_worker_event_queue != NULL);
  PBL_ASSERTN(s_from_kernel_event_queue != NULL);

  // Initialize message queues with static buffers
  k_msgq_init(s_kernel_event_queue, s_kernel_event_buffer, sizeof(PebbleEvent), MAX_KERNEL_EVENTS);
  k_msgq_init(s_from_app_event_queue, s_from_app_event_buffer, sizeof(PebbleEvent), MAX_FROM_APP_EVENTS);
  k_msgq_init(s_from_worker_event_queue, s_from_worker_event_buffer, sizeof(PebbleEvent), MAX_FROM_WORKER_EVENTS);
  k_msgq_init(s_from_kernel_event_queue, s_from_kernel_event_buffer, sizeof(PebbleEvent), MAX_FROM_KERNEL_MAIN_EVENTS);

  // Initialize poll events for waiting on multiple queues
  poll_events[0] = K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, s_kernel_event_queue, 0);
  poll_events[1] = K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, s_from_app_event_queue, 0);
  poll_events[2] = K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, s_from_worker_event_queue, 0);
}

//! Get the from_process queue for a specific task
QueueHandle_t event_get_to_kernel_queue(PebbleTask task) {
  if (task == PebbleTask_App) {
    return s_from_app_event_queue;
  } else if (task == PebbleTask_Worker) {
    return s_from_worker_event_queue;
  } else if (task == PebbleTask_KernelMain) {
    return s_from_kernel_event_queue;
  } else if ((task == PebbleTask_NewTimers) || (task == PebbleTask_KernelBackground)) {
    return s_kernel_event_queue;
  } else {
    WTF;
    return NULL;
  }
}


//! Decode a bit more information out about an event and pack it into a uint32_t
static uint32_t prv_get_fancy_type_from_event(const PebbleEvent *event) {
  if (event->type == PEBBLE_CALLBACK_EVENT) {
    return (uint32_t) event->callback.callback;
  }
  return event->type;
}

static void prv_log_event_put_failure(const char *queue_name, uintptr_t saved_lr, const PebbleEvent *event) {
  PBL_LOG(LOG_LEVEL_ERROR, "Error, %s queue full. Type %u", queue_name, event->type);

  RebootReason reason = {
    .code = RebootReasonCode_EventQueueFull,
    .event_queue = {
      .push_lr = saved_lr,
      .current_event = s_current_event,
      .dropped_event = prv_get_fancy_type_from_event(event)
    }
  };
  reboot_reason_set(&reason);
}

static bool prv_event_put_isr(QueueHandle_t queue, const char* queue_type, uintptr_t saved_lr,
                                  PebbleEvent* event) {
  PBL_ASSERTN(queue);

  struct k_msgq *msgq = (struct k_msgq *)queue;
  int result = k_msgq_put_from_isr(msgq, event, K_NO_WAIT);

  if (result != 0) {
    prv_log_event_put_failure(queue_type, saved_lr, event);

#ifdef NO_WATCHDOG
    enable_mcu_debugging();
    while (1);
#endif

    reset_due_to_software_failure();
  }

  return false; // No context switch needed in Zephyr ISR
}

static bool prv_try_event_put(QueueHandle_t queue, PebbleEvent *event) {
  PBL_ASSERTN(queue);
  struct k_msgq *msgq = (struct k_msgq *)queue;
  return (k_msgq_put(msgq, event, K_MSEC(3000)) == 0);
}

static void prv_event_put(QueueHandle_t queue,
                          const char* queue_type,
                          uintptr_t saved_lr,
                          PebbleEvent* event) {
  PBL_ASSERTN(queue);
  struct k_msgq *msgq = (struct k_msgq *)queue;

  if (k_msgq_put(msgq, event, K_MSEC(3000)) != 0) {
    // We waited a reasonable amount of time here before failing. We don't want to wait too long because
    // if the queue really is stuck we'll just get a watchdog reset, which will be harder to debug than
    // just dieing here. However, we want to wait a non-zero amount of time to provide for a little bit
    // of backup to occur before killing ourselves.

    prv_log_event_put_failure(queue_type, saved_lr, event);

#if EVENT_DEBUG
    // prv_queue_dump(queue); // Not implemented for Zephyr
#endif

    reset_due_to_software_failure();
  }
}

void event_deinit(PebbleEvent* event) {
  void **buffer = event_get_buffer(event);
  if (buffer && *buffer) {
    kernel_free(*buffer);
    *buffer = NULL;
  }
}

void event_put(PebbleEvent* event) {
  register uintptr_t lr __asm("lr");
  uintptr_t saved_lr = lr;
  // If we are posting from the KernelMain task, use the dedicated s_from_kernel_event_queue queue for that
  // See comments above where s_from_kernel_event_queue is declared.
  if (pebble_task_get_current() == PebbleTask_KernelMain) {
    return prv_event_put(s_from_kernel_event_queue, "from_kernel", saved_lr, event);
  } else {
    return prv_event_put(s_kernel_event_queue, "kernel", saved_lr, event);
  }
}

bool event_put_isr(PebbleEvent* event) {
  register uintptr_t lr __asm("lr");
  uintptr_t saved_lr = lr;

  return prv_event_put_isr(s_kernel_event_queue, "kernel", saved_lr, event);
}

void event_put_from_process(PebbleTask task, PebbleEvent* event) {
  register uintptr_t lr __asm("lr");
  uintptr_t saved_lr = lr;

  QueueHandle_t queue = event_get_to_kernel_queue(task);
  prv_event_put(queue, "from app", saved_lr, event);
}

bool event_try_put_from_process(PebbleTask task, PebbleEvent* event) {
  QueueHandle_t queue = event_get_to_kernel_queue(task);
  return prv_try_event_put(queue, event);
}

bool event_take_timeout(PebbleEvent* event, int timeout_ms) {
  s_current_event = 0;

  // We must prioritize the from_kernel queue and always empty that first in order to avoid deadlocks in
  // KernelMain. See comments at top of file where s_from_kernel_event_queue is declared.

  // Check the from_kernel queue first to see if we posted any events to ourself.
  int result = k_msgq_get(s_from_kernel_event_queue, event, K_NO_WAIT);
  if (result == 0) {
    s_current_event = prv_get_fancy_type_from_event(event);
    return true;
  }

  // Wait for either the from_app, from_worker, or kernel queue to be ready.
  // Copy poll events to avoid modifying the original array
  struct k_poll_event local_poll_events[3];
  memcpy(local_poll_events, poll_events, sizeof(poll_events));

  result = k_poll(local_poll_events, 3, K_MSEC(timeout_ms));
  if (result != 0) {
    return false; // No event received within timeout
  }

  // Always service the kernel queue first. This prevents a misbehaving app from starving us.
  // If we're a little lazy servicing the app, the app will just block itself when the queue gets full.
  result = k_msgq_get(s_kernel_event_queue, event, K_NO_WAIT);
  if (result == 0) {
    s_current_event = prv_get_fancy_type_from_event(event);
    return true;
  }

  // Process the from_app queue
  result = k_msgq_get(s_from_app_event_queue, event, K_NO_WAIT);
  if (result == 0) {
    s_current_event = prv_get_fancy_type_from_event(event);
    return true;
  }

  // Process the from_worker queue
  result = k_msgq_get(s_from_worker_event_queue, event, K_NO_WAIT);
  if (result == 0) {
    s_current_event = prv_get_fancy_type_from_event(event);
    return true;
  }

  // If there was nothing in any of the queues, return false
  return false;
}

void **event_get_buffer(PebbleEvent *event) {
  switch (event->type) {
    case PEBBLE_SYS_NOTIFICATION_EVENT:
      if (event->sys_notification.type == NotificationActionResult) {
        return (void **)&event->sys_notification.action_result;
      } else if ((event->sys_notification.type == NotificationAdded) ||
                 (event->sys_notification.type == NotificationRemoved) ||
                 (event->sys_notification.type == NotificationActedUpon)) {
        return (void **)&event->sys_notification.notification_id;
      }
      break;

    case PEBBLE_BLOBDB_EVENT:
      return (void **)&event->blob_db.key;

    case PEBBLE_BT_PAIRING_EVENT:
      if (event->bluetooth.pair.type ==
          PebbleBluetoothPairEventTypePairingUserConfirmation) {
        return (void **)&event->bluetooth.pair.confirmation_info;
      }
      break;

    case PEBBLE_APP_LAUNCH_EVENT:
      return (void **)&event->launch_app.data;

    case PEBBLE_VOICE_SERVICE_EVENT:
      return (void **)&event->voice_service.data;

    case PEBBLE_REMINDER_EVENT:
      return (void **)&event->reminder.reminder_id;

    case PEBBLE_BLE_GATT_CLIENT_EVENT:
      if (event->bluetooth.le.gatt_client.subtype == PebbleBLEGATTClientEventTypeServiceChange) {
        return (void **)(&event->bluetooth.le.gatt_client_service.info);
      }
      break;
#ifdef MANUFACTURING_FW
    case PEBBLE_HRM_EVENT:
      if (event->hrm.event_type == HRMEvent_CTR) {
        return (void **)(&event->hrm.ctr);
      } else if (event->hrm.event_type == HRMEvent_Leakage) {
        return (void **)(&event->hrm.leakage);
      }
      break;
#endif
    case PEBBLE_APP_GLANCE_EVENT:
      return (void **)&event->app_glance.app_uuid;

    case PEBBLE_TIMELINE_PEEK_EVENT:
      return (void **)&event->timeline_peek.item_id;

    default:
      break; // Nothing to do!
  }

  return NULL;
}

void event_cleanup(PebbleEvent* event) {
  event_deinit(event);

#ifndef RELEASE
  // Hopefully this will catch some use after free evil
  *event = (PebbleEvent){};
#endif
}

void event_reset_from_process_queue(PebbleTask task) {
  // In Zephyr, we don't use queue sets like FreeRTOS, so this function is simplified
  // We just need to clean up and reset the appropriate queue based on the task type
  
  struct k_msgq *reset_queue = NULL;
  
  if (task == PebbleTask_App) {
    reset_queue = s_from_app_event_queue;
  } else if (task == PebbleTask_Worker) {
    reset_queue = s_from_worker_event_queue;
  } else {
    WTF;
    return;
  }
  
  // Clean up and reset the specified queue
  event_queue_cleanup_and_reset((QueueHandle_t)reset_queue);
  
  // In Zephyr, we don't need to manage queue sets, so no additional steps are needed
}


QueueHandle_t event_kernel_to_kernel_event_queue(void) {
  return s_from_kernel_event_queue;
}

BaseType_t event_queue_cleanup_and_reset(QueueHandle_t queue) {
  struct k_msgq *msgq = (struct k_msgq *)queue;
  PebbleEvent event;

  // Process all events in the queue
  while (k_msgq_get(msgq, &event, K_NO_WAIT) == 0) {
    // event service does some book-keeping about events, notify it that we're dropping these.
    sys_event_service_cleanup(&event);
#if !RECOVERY_FW
    // app outbox service messages need to be cleaned up:
    app_outbox_service_cleanup_event(&event);
#endif
    // cleanup the event, free associated memory if applicable
    event_cleanup(&event);
  }

  // In Zephyr, we don't need to explicitly reset the message queue
  // The above loop has already emptied it
  return pdPASS;
}
