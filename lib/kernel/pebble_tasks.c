/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "kernel/pebble_tasks.h"

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
  // Simplified implementation for Zephyr
  // We don't have an accurate way to get free stack space in Zephyr
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

struct k_msgq* pebble_task_get_to_queue(PebbleTask task) {
  // This function returns the queue for sending events to the given task
  // In our Zephyr implementation, we map this to the appropriate event queue
  switch (task) {
    case PebbleTask_KernelMain:
      // event_get_to_kernel_queue() is not implemented yet in Zephyr port
      // Return NULL for now
      return NULL;
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

void pebble_task_create(PebbleTask pebble_task, TaskParameters_t *task_params, k_tid_t *handle) {
  // TaskParameters_t is defined as void* in the header, so we can't access its members directly
  // For now, we'll implement a simplified version that creates a thread with default parameters
  printk("pebble_task_create: Not fully implemented\n");

  // We need more information to create a proper thread, but for now we'll just return NULL
  if (handle != NULL) {
    *handle = NULL;
  }

    k_tid_t tid = k_thread_create(&task_params->thread_data, task_params->stack,
        task_params->stack_size, task_params->func,
        task_params->arg,
        NULL, NULL,
        task_params->prio,
        0,
        K_NO_WAIT);

//    if (task_params->name) {
//      k_thread_name_set(tid, task_params->name);
//    }

    if (handle) {
      *handle = tid;
    }

    prv_task_register(pebble_task, tid);
}

void pebble_task_configure_idle_task(void) {
  // In Zephyr, the idle task is configured by the kernel
  // No additional configuration is needed here
}
