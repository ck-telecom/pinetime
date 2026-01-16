/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "kernel/event_loop.h"
#include "kernel/events.h"
#include "services/common/event_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "system/logging.h"

// Force quit constants
static const uint32_t FORCE_QUIT_HOLD_MS = 1500;
static struct k_timer s_back_hold_timer;

// Quick press detection for core dump
static const uint32_t BACK_QUICKPRESS_INTERVAL_TICKS = 300;
static const int BACK_QUICKPRESS_COREDUMP_PRESSES = 10;
static uint64_t s_back_quickpress_last = 0;
static int s_back_quickpress_count = 0;

// Popup blocking
static bool launcher_is_popup_event(PebbleEvent* e) {
  switch (e->type) {
    case PEBBLE_SYS_NOTIFICATION_EVENT:
    case PEBBLE_ALARM_CLOCK_EVENT:
    case PEBBLE_BATTERY_CONNECTION_EVENT:
    case PEBBLE_BATTERY_STATE_CHANGE_EVENT:
      return true;
    default:
      return false;
  }
}

static int s_block_popup_count = 0;

// Force quit cancellation flag
static bool s_force_quit_was_cancelled = false;

// Metric for cancelled force quits
uint32_t metric_firm_425_back_button_long_presses_cancelled = 0;

void launcher_task_add_callback(void (*callback)(void *data), void *data) {
  PebbleEvent event = {
    .type = PEBBLE_CALLBACK_EVENT,
    .callback = {
      .callback = callback,
      .data = data,
    },
  };
  event_put(&event);
}

bool launcher_task_is_current_task(void) {
  return (pebble_task_get_current() == PebbleTask_KernelMain);
}

void launcher_block_popups(bool block) {
  if (block) {
    s_block_popup_count++;
  } else {
    if (s_block_popup_count > 0) {
      s_block_popup_count--;
    }
  }
}

bool launcher_popups_are_blocked(void) {
  return s_block_popup_count > 0;
}

// Forward declaration
static void launcher_force_quit_app(void *data);

// Timer expiry callback for force quit
static void back_button_force_quit_handler(struct k_timer *timer) {
  launcher_task_add_callback(launcher_force_quit_app, NULL);
}

void launcher_cancel_force_quit(void) {
  s_force_quit_was_cancelled = true;
  k_timer_stop(&s_back_hold_timer);
}

// Force quit app function
static void launcher_force_quit_app(void *data) {
  if (s_force_quit_was_cancelled) {
    printk("NewTimer event fired for force quit, but the back button was released before we went to deal with it!\n");
    metric_firm_425_back_button_long_presses_cancelled++;
    return;
  }

  printk("Force killing app.\n");
  // TODO: Implement app_manager_force_quit_to_launcher()
}

// Minimal event handler for basic events
static void prv_minimal_event_handler(PebbleEvent* e) {
  switch (e->type) {
    case PEBBLE_CALLBACK_EVENT:
      if (e->callback.callback != NULL) {
        e->callback.callback(e->callback.data);
      }
      return;

    case PEBBLE_RENDER_REQUEST_EVENT:
      // Handle render request events
      return;

    case PEBBLE_RENDER_READY_EVENT:
      // Handle render ready events
      return;

    case PEBBLE_RENDER_FINISHED_EVENT:
      // Handle render finished events
      return;

    case PEBBLE_ACCEL_SHAKE_EVENT:
      // Handle shake events
      return;

    default:
      return;
  }
}

// Extended event handler for more complex events
static void prv_extended_event_handler(PebbleEvent* e) {
  switch (e->type) {
    case PEBBLE_SET_TIME_EVENT:
      // Handle time set events
      return;

    case PEBBLE_SYSTEM_MESSAGE_EVENT:
      // Handle system messages
      return;

    default:
      return;
  }
}

// Tasks that have to be done in between each event
static void event_loop_upkeep(void) {
  // Implement any upkeep tasks here
}

// Main event handler
static void prv_handle_event(PebbleEvent *e) {
  prv_minimal_event_handler(e);

  // Block popup events if requested
  if (s_block_popup_count > 0) {
    if (launcher_is_popup_event(e)) {
      return;
    }
  }

  // Handle extended events
  prv_extended_event_handler(e);

  // Pass the event to the event service
  event_service_handle_event(e);
}

// Main loop initialization
static void prv_launcher_main_loop_init(void) {
  // Initialize the back button timer
  k_timer_init(&s_back_hold_timer, back_button_force_quit_handler, NULL);

  // Initialize event service
  event_service_system_init();

  // TODO: Initialize other services as needed
}

void launcher_main_loop(void) {
  printk("Starting Launcher Main Loop\n");

  // Initialize the main loop
  prv_launcher_main_loop_init();

  while (1) {
    // We make this PebbleEvent static to save stack space
    static PebbleEvent e;
    if (event_take_timeout(&e, 1000)) {
      const PebbleTaskBitset kernel_main_task_bit = (1 << PebbleTask_KernelMain);
      const bool is_not_masked_out_from_kernel_main = !(e.task_mask & kernel_main_task_bit);

      if (is_not_masked_out_from_kernel_main) {
        prv_handle_event(&e);
      }

      // Clean up the event after handling
      event_cleanup(&e);

      // Perform any upkeep tasks
      event_loop_upkeep();
    }
  }
  __builtin_unreachable();
}
