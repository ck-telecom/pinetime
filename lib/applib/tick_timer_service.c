/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "tick_timer_service.h"
#include "tick_timer_service_private.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <time.h>

// Define a global state for the tick timer service
static TickTimerServiceState g_tick_timer_state;
static struct k_timer g_tick_timer;
static bool g_timer_running = false;

// ----------------------------------------------------------------------------------------------------
static TickTimerServiceState* prv_get_state(void) {
  // For Zephyr implementation, use a single global state
  return &g_tick_timer_state;
}

// Timer callback function
static void tick_timer_callback(struct k_timer *timer_id) {
  TickTimerServiceState *state = prv_get_state();
  if (state->handler == NULL) {
    return;
  }

  TimeUnits units_changed = 0;
  time_t current_time = time(NULL);
  struct tm currtime;
  localtime_r(&current_time, &currtime);

  if (!state->first_tick) {
    if (state->last_time.tm_sec != currtime.tm_sec) {
      units_changed |= SECOND_UNIT;
    }
    if (state->last_time.tm_min != currtime.tm_min) {
      units_changed |= MINUTE_UNIT;
    }
    if (state->last_time.tm_hour != currtime.tm_hour) {
      units_changed |= HOUR_UNIT;
    }
    if (state->last_time.tm_mday != currtime.tm_mday) {
      units_changed |= DAY_UNIT;
    }
    if (state->last_time.tm_mon != currtime.tm_mon) {
      units_changed |= MONTH_UNIT;
    }
    if (state->last_time.tm_year != currtime.tm_year) {
      units_changed |= YEAR_UNIT;
    }
  }
  state->last_time = currtime;
  state->first_tick = false;

  if ((state->tick_units & units_changed) || (units_changed == 0)) {
    state->handler(&currtime, units_changed);
  }
}

void tick_timer_service_init(void) {
  TickTimerServiceState *state = prv_get_state();
  state->handler = NULL;
  
  // Initialize the Zephyr timer
  k_timer_init(&g_tick_timer, tick_timer_callback, NULL);
  
  printk("Tick timer service initialized\n");
}

void tick_timer_service_subscribe(TimeUnits tick_units, TickHandler handler) {
  TickTimerServiceState *state = prv_get_state();
  state->handler = handler;
  state->tick_units = tick_units;
  state->first_tick = true;
  
  // Get current time for initial state
  time_t current_time = time(NULL);
  localtime_r(&current_time, &state->last_time);
  
  // Start the timer with 1 second interval
  if (!g_timer_running) {
    k_timer_start(&g_tick_timer, K_SECONDS(1), K_SECONDS(1));
    g_timer_running = true;
    printk("Tick timer started\n");
  }
}

void tick_timer_service_unsubscribe(void) {
  TickTimerServiceState *state = prv_get_state();
  state->handler = NULL;
  
  // Stop the timer if no subscribers
  if (g_timer_running) {
    k_timer_stop(&g_tick_timer);
    g_timer_running = false;
    printk("Tick timer stopped\n");
  }
}

void tick_timer_service_state_init(TickTimerServiceState *state) {
  *state = (TickTimerServiceState) {
    .handler = NULL,
    .tick_units = 0,
    .first_tick = true,
  };
}

void tick_timer_service_reset(void) {
  TickTimerServiceState *state = prv_get_state();
  state->handler = NULL;
  state->tick_units = 0;
  state->first_tick = true;
}
