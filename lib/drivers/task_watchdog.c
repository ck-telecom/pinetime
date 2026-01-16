/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "drivers/task_watchdog.h"

#include "kernel/event_loop.h"


#include "services/common/evented_timer.h"
#include "services/common/system_task.h"
#include "system/bootbits.h"
#include "system/logging.h"
#include "system/passert.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>

#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#ifdef NO_WATCHDOG
#include "debug/setup.h"
#endif

#define WDT_NODE DT_ALIAS(watchdog0)

// Register logger
LOG_MODULE_DECLARE(task_watchdog, LOG_LEVEL_DBG);

#define APP_THROTTLE_TIME_MS 300

// The App Throttle Timer
static EventedTimerID s_throttle_timer_id = EVENTED_TIMER_INVALID_ID;

// Task watchdog channel IDs for each Pebble task
static int s_task_wdt_channels[NumPebbleTask] = { -1 };

// Default task watchdog timeout in milliseconds
#define TASK_WDT_DEFAULT_TIMEOUT_MS 5000

static void prv_system_task_starved_callback(void *data);

static void prv_app_task_throttle_end(void *data) {
  // In Zephyr, we don't directly set thread priorities like in FreeRTOS
  // This function may need to be reimplemented based on Zephyr's thread management
  LOG_DBG("Ending App Throttling");
}

static void prv_app_task_throttle_start(void) {
  // In Zephyr, we don't directly set thread priorities like in FreeRTOS
  // This function may need to be reimplemented based on Zephyr's thread management
}

static void prv_system_task_starved_callback(void *data) {
  if (system_task_is_ready_to_run() || (system_task_get_current_callback() != NULL)) {
    // check if system task is ready to go or is already running a callback.
    // If it's ready to run, we definitely want to throttle the app task.
    // Or, if it's blocked in a callback, there's a chance it could be waiting for a mutex held by
    // the background worker and the worker won't be able to release it until we throttle the app
    // to give the worker some time.
    prv_app_task_throttle_start();
    // throttle the app task for APP_THROTTLE_TIME_MS to give the system task some runtime
    s_throttle_timer_id = evented_timer_register_or_reschedule(
        s_throttle_timer_id, APP_THROTTLE_TIME_MS, prv_app_task_throttle_end, NULL);
  }
}

// Task watchdog timeout callback
static void prv_task_wdt_timeout_callback(int channel_id, void *user_data) {
#if 0
  PebbleTask task = (PebbleTask)(uintptr_t)user_data;
  const char *task_name = pebble_task_get_name(task);

  LOG_ERR("Task watchdog timeout for %s", task_name);

  // Create reboot reason with watchdog information
  RebootReason reboot_reason = {
    .code = RebootReasonCode_Watchdog,
    .data8 = { (uint8_t)task, 0 } // Store task ID in first byte
  };
  reboot_reason_set(&reboot_reason);

  // Handle system task starvation
  if (task == PebbleTask_KernelBackground) {
    // Put system task callback using from ISR variant
    PebbleEvent event = {
      .type = PEBBLE_CALLBACK_EVENT,
      .callback = {
        .callback = prv_system_task_starved_callback,
        .data = NULL,
      },
    };
    event_put_isr(&event);
  }

  // If we're in a critical state, reset the system
  reset_due_to_software_failure();
#endif
}

// ============================================================================================================
// Public functions

// -------------------------------------------------------------------------------------------------
// Setup task watchdogs for all Pebble tasks
void task_watchdog_init(void) {
  int ret;
  const struct device *const hw_wdt_dev = DEVICE_DT_GET_OR_NULL(WDT_NODE);

  // Initialize app throttling timer (no need to create, evented_timer_register handles it)

#ifdef CONFIG_TASK_WDT
  LOG_INF("Task watchdog subsystem initializing...");

  // Initialize the task watchdog subsystem with hardware watchdog if available
  if (!device_is_ready(hw_wdt_dev)) {
    LOG_INF("Hardware watchdog not ready; using software-only task watchdog");
    ret = task_wdt_init(NULL);
  } else {
    LOG_INF("Using hardware watchdog %s", hw_wdt_dev->name);
    ret = task_wdt_init(hw_wdt_dev);
  }

  if (ret != 0) {
    LOG_ERR("Failed to initialize task watchdog subsystem: %d", ret);
    return;
  }

  LOG_INF("Task watchdog subsystem initialized");

  // Initialize watchdog channels for each task
  for (int i = 0; i < NumPebbleTask; i++) {
    PebbleTask task = (PebbleTask)i;

    // Skip invalid tasks
    if (task == PebbleTask_Unknown) {
      continue;
    }

    // Add task watchdog channel - Zephyr API: timeout_ms, callback, user_data
    s_task_wdt_channels[i] = task_wdt_add(TASK_WDT_DEFAULT_TIMEOUT_MS,
                                          prv_task_wdt_timeout_callback,
                                          (void *)(uintptr_t)task);

    if (s_task_wdt_channels[i] < 0) {
      LOG_ERR("Failed to add task watchdog for %s: %d",
             pebble_task_get_name(task), s_task_wdt_channels[i]);
      s_task_wdt_channels[i] = -1;
      continue;
    }

    LOG_INF("Added task watchdog channel %d for %s with timeout %d ms",
           s_task_wdt_channels[i], pebble_task_get_name(task), TASK_WDT_DEFAULT_TIMEOUT_MS);
  }
#else
  LOG_WRN("Task watchdog is disabled (CONFIG_TASK_WDT not set)");
#endif
}

void task_watchdog_feed(void) {
  // Feed all task watchdogs
  task_watchdog_bit_set_all();
}

void task_watchdog_bit_set_all(void) {
#if defined(CONFIG_TASK_WDT)
  // Feed all task watchdogs
  for (int i = 0; i < NumPebbleTask; i++) {
    if (s_task_wdt_channels[i] >= 0) {
      task_wdt_feed(s_task_wdt_channels[i]);
    }
  }
#endif
}

void task_watchdog_bit_set(PebbleTask task) {
#if defined(CONFIG_TASK_WDT)
  if (task < NumPebbleTask && s_task_wdt_channels[task] >= 0) {
    task_wdt_feed(s_task_wdt_channels[task]);
    LOG_DBG("Fed task watchdog for %s (channel %d)",
           pebble_task_get_name(task), s_task_wdt_channels[task]);
  }
#endif
}

bool task_watchdog_mask_get(PebbleTask task) {
  // In Zephyr task_wdt, we don't use a mask, all tasks are watched by default
  return true;
}

void task_watchdog_mask_set(PebbleTask task) {
  if (task < NumPebbleTask && s_task_wdt_channels[task] < 0) {
    s_task_wdt_channels[task] = task_wdt_add(TASK_WDT_DEFAULT_TIMEOUT_MS,
                                          prv_task_wdt_timeout_callback,
                                          (void *)(uintptr_t)task);
    if (s_task_wdt_channels[task] >= 0) {
      LOG_DBG("Added task watchdog channel %d for %s with timeout %d ms",
             s_task_wdt_channels[task], pebble_task_get_name(task), TASK_WDT_DEFAULT_TIMEOUT_MS);
    } else {
      LOG_ERR("Failed to add task watchdog for %s: %d",
             pebble_task_get_name(task), s_task_wdt_channels[task]);
      s_task_wdt_channels[task] = -1;
    }
  }
}

void task_watchdog_mask_clear(PebbleTask task) {
  if (task < NumPebbleTask && s_task_wdt_channels[task] >= 0) {
    task_wdt_delete(s_task_wdt_channels[task]);
    s_task_wdt_channels[task] = -1;
    LOG_DBG("Cleared task watchdog channel %d for %s",
           s_task_wdt_channels[task], pebble_task_get_name(task));
  }
}

void task_watchdog_pause(unsigned int seconds) {
  task_wdt_suspend();
}

void task_watchdog_resume(void) {
  task_wdt_resume();
}

void task_watchdog_step_elapsed_time_ms(uint32_t elapsed_ms) {
  // In Zephyr task_wdt, this function is a no-op as the watchdog
  // handles elapsed time internally
}
