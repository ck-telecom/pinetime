/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "pebble_tasks.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

/* Track task handles - map PebbleTask to Zephyr thread ID */
k_tid_t g_task_handles[NumPebbleTask] = { NULL };

static void prv_task_register(PebbleTask task, k_tid_t task_handle) {
  if (task < NumPebbleTask) {
    g_task_handles[task] = task_handle;
  }
}

void pebble_task_register(PebbleTask task, k_tid_t task_handle) {
  prv_task_register(task, task_handle);
}

void pebble_task_unregister(PebbleTask task) {
  if (task < NumPebbleTask) {
    g_task_handles[task] = NULL;
  }
}

const char* pebble_task_get_name(PebbleTask task) {
  if (task >= NumPebbleTask) {
    if (task == PebbleTask_Unknown) {
      return "Unknown";
    }
    printk("Invalid task: %d\n", task);
    return "Unknown";
  }

  k_tid_t task_handle = g_task_handles[task];
  if (!task_handle) {
    return "Unknown";
  }

  const char *name = k_thread_name_get(task_handle);
  return name ? name : "Unknown";
}

// NOTE: The logging support calls toupper() this character if the task is currently running privileged, so
//  these identifiers should be all lower case and case-insensitive.
char pebble_task_get_char(PebbleTask task) {
  switch (task) {
  case PebbleTask_KernelMain:
    return 'm';
  case PebbleTask_KernelBackground:
    return 's';
  case PebbleTask_Worker:
    return 'w';
  case PebbleTask_App:
    return 'a';
  case PebbleTask_BTHost:
    return 'b';
  case PebbleTask_BTController:
    return 'c';
  case PebbleTask_BTHCI:
    return 'd';
  case PebbleTask_NewTimers:
    return 't';
  case PebbleTask_PULSE:
    return 'p';
  case NumPebbleTask:
  case PebbleTask_Unknown:
    ;
  }

  return '?';
}

PebbleTask pebble_task_get_current(void) {
  k_tid_t current_thread = k_current_get();
  return pebble_task_get_task_for_handle(current_thread);
}

PebbleTask pebble_task_get_task_for_handle(k_tid_t task_handle) {
  for (int i = 0; i < NumPebbleTask; i++) {
    if (g_task_handles[i] == task_handle) {
      return (PebbleTask)i;
    }
  }
  return PebbleTask_Unknown;
}

k_tid_t pebble_task_get_handle_for_task(PebbleTask task) {
  if (task < NumPebbleTask) {
    return g_task_handles[task];
  }
  return NULL;
}

static uint16_t prv_task_get_stack_free(PebbleTask task) {
  // Not implemented in Zephyr port
  return 0;
}

void pebble_task_suspend(PebbleTask task) {
  k_tid_t thread = pebble_task_get_handle_for_task(task);
  if (thread != NULL) {
    k_thread_suspend(thread);
  }
}

void analytics_external_collect_stack_free(void) {
  // Not implemented in Zephyr port
}

QueueHandle_t pebble_task_get_to_queue(PebbleTask task) {
  // This function returns the queue for sending events to the given task
  // In our Zephyr implementation, we map this to the appropriate event queue
  switch (task) {
    case PebbleTask_KernelMain:
      return event_get_to_kernel_queue(pebble_task_get_current());
    case PebbleTask_Worker:
      // Worker task queue will be implemented later
      return NULL;
    case PebbleTask_App:
      // App task queue will be implemented later
      return NULL;
    case PebbleTask_KernelBackground:
      return NULL;
    default:
      printk("Invalid task for get_to_queue: %d\n", task);
      return NULL;
  }
}

// TaskParameters_t is assumed to be a FreeRTOS structure - we'll provide a simplified implementation
// that extracts the necessary fields for Zephyr's k_thread_create
void pebble_task_create(PebbleTask pebble_task, void *task_params, k_tid_t *handle) {
  if (task_params == NULL) {
    printk("pebble_task_create: task_params is NULL\n");
    return;
  }

  TaskParameters_t *params = (TaskParameters_t *)task_params;
  k_tid_t thread_id;

  // Map FreeRTOS priority to Zephyr priority
  // FreeRTOS: higher number = higher priority
  // Zephyr: lower number = higher priority (0 is highest, K_PRIO_COOP(15) is lowest for cooperative threads)
  int zephyr_prio = K_PRIO_COOP(10); // Default priority
  if (params->uxPriority != 0) {
    // Extract priority value without privilege bit
    UBaseType_t freertos_prio = params->uxPriority & ~portPRIVILEGE_BIT;
    // Map 0-31 to Zephyr cooperative priorities (15-0)
    zephyr_prio = K_PRIO_COOP(15 - (freertos_prio * 15) / 31);
  }

  // Create thread using Zephyr API
  // Note: In Zephyr, stack size is in bytes, not words
  // We'll assume that usStackDepth is in 4-byte words
  size_t stack_size = params->usStackDepth * 4;

  // If stack buffer is provided, use it; otherwise, let Zephyr allocate stack
  if (params->puxStackBuffer != NULL) {
    // Use provided stack buffer
    thread_id = k_thread_create(
      NULL, // No need for thread control block pointer in Zephyr
      params->puxStackBuffer, // Stack buffer
      stack_size, // Stack size in bytes
      (k_thread_entry_t)params->pvTaskCode, // Thread entry function
      params->pvParameters, // Task parameters
      NULL,
      NULL,
      zephyr_prio, // Priority
      0, // No thread options
      K_NO_WAIT // Start immediately
    );
  } else {
    // Let Zephyr allocate stack
    thread_id = k_thread_create(
      NULL, // No need for thread control block pointer in Zephyr
      NULL, // No stack buffer provided, Zephyr will allocate
      stack_size, // Stack size in bytes
      (k_thread_entry_t)params->pvTaskCode, // Thread entry function
      params->pvParameters, // Task parameters
      NULL,
      NULL,
      zephyr_prio, // Priority
      0, // No thread options
      K_NO_WAIT // Start immediately
    );
  }

  // Register the task
  if (thread_id != NULL) {
    // Set thread name if provided
    if (params->pcName != NULL) {
      k_thread_name_set(thread_id, params->pcName);
    }

    pebble_task_register(pebble_task, thread_id);

    // Return thread handle if requested
    if (handle != NULL) {
      *handle = thread_id;
    }
  } else {
    printk("pebble_task_create: Failed to create thread %s\n", params->pcName);
  }
}

void pebble_task_configure_idle_task(void) {
  // In Zephyr, the idle task is configured by the kernel
  // No additional configuration is needed here
}
