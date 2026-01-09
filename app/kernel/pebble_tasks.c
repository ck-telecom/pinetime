/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "pebble_tasks.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

/* Task name strings */
static const char *const task_names[] = {
    "KernelMain",
    "KernelBackground",
    "Worker",
    "App",
    "BTHost",
    "BTController",
    "BTHCI",
    "NewTimers",
    "PULSE"
};

/* Track task handles - map PebbleTask to Zephyr thread ID */
static k_tid_t g_task_handles[NumPebbleTask] = { NULL };

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
  if (task < NumPebbleTask) {
    return task_names[task];
  }
  return "Unknown";
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
  // Not implemented in Zephyr port
  return NULL;
}

// TaskParameters_t is assumed to be a FreeRTOS structure - we'll provide a simplified implementation
// that extracts the necessary fields for Zephyr's k_thread_create
void pebble_task_create(PebbleTask pebble_task, void *task_params, k_tid_t *handle) {
  // Note: This is a simplified implementation. In a real port, you would need to:
  // 1. Map FreeRTOS TaskParameters_t to Zephyr thread creation parameters
  // 2. Handle stack allocation and configuration
  // 3. Set up proper thread priorities
  // 4. Configure memory protection if needed

  // For now, we'll just log an error indicating this function needs proper implementation
  printk("pebble_task_create: Not fully implemented for Zephyr\n");
}

void pebble_task_configure_idle_task(void) {
  // In Zephyr, the idle task is configured by the kernel
  // No additional configuration is needed here
}
