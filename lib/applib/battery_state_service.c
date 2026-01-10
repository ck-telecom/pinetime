/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "battery_state_service.h"
#include "battery_state_service_private.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <battery.h>

// Define a global state for the battery state service
static BatteryStateServiceState g_battery_state;
static struct k_timer g_battery_timer;
static bool g_timer_running = false;
static BatteryChargeState g_last_battery_state;

// ----------------------------------------------------------------------------------------------------
static BatteryStateServiceState* prv_get_state(void) {
  // For Zephyr implementation, use a single global state
  return &g_battery_state;
}

// Calculate battery charge percentage based on voltage
static uint8_t calculate_charge_percent(int voltage_mv) {
  // Simple linear approximation
  // Typical lithium-ion battery voltage range: 3.0V (0%) to 4.2V (100%)
  if (voltage_mv <= 3000) return 0;
  if (voltage_mv >= 4200) return 100;

  // Calculate percentage between 3.0V and 4.2V
  return (uint8_t)((voltage_mv - 3000) * 100 / (4200 - 3000));
}

// Timer callback function to check battery state periodically
static void battery_timer_callback(struct k_timer *timer_id) {
  BatteryStateServiceState *state = prv_get_state();
  if (state->handler == NULL) {
    return;
  }

  // Get current battery state from battery driver
  int voltage_mv = battery_get_millivolts();
  bool is_charging = battery_charge_controller_thinks_we_are_charging();
  bool is_plugged = battery_is_usb_connected();

  BatteryChargeState current_state = {
    .charge_percent = calculate_charge_percent(voltage_mv),
    .is_charging = is_charging,
    .battery_voltage = (float)voltage_mv / 1000.0f, // Convert from mV to V
    .is_plugged = is_plugged,
  };

  // Check if battery state has changed
  if (g_last_battery_state.charge_percent != current_state.charge_percent ||
      g_last_battery_state.is_charging != current_state.is_charging ||
      g_last_battery_state.is_plugged != current_state.is_plugged) {

    // Update last known state
    g_last_battery_state = current_state;

    // Notify the handler
    state->handler(current_state);
  }
}

void battery_state_service_init(void) {
  BatteryStateServiceState *state = prv_get_state();
  state->handler = NULL;

  // Initialize the Zephyr timer - check battery state every 10 seconds
  k_timer_init(&g_battery_timer, battery_timer_callback, NULL);

  // Initialize battery driver
  battery_init();

  // Get initial battery state
  int voltage_mv = battery_get_millivolts();
  g_last_battery_state = (BatteryChargeState) {
    .charge_percent = calculate_charge_percent(voltage_mv),
    .is_charging = battery_charge_controller_thinks_we_are_charging(),
    .battery_voltage = (float)voltage_mv / 1000.0f, // Convert from mV to V
    .is_plugged = battery_is_usb_connected(),
  };

  printk("Battery state service initialized\n");
}

void battery_state_service_subscribe(BatteryStateHandler handler) {
  BatteryStateServiceState *state = prv_get_state();
  state->handler = handler;

  // Start the timer if not already running
  if (!g_timer_running) {
    k_timer_start(&g_battery_timer, K_SECONDS(1), K_SECONDS(10)); // Initial check after 1s, then every 10s
    g_timer_running = true;
    printk("Battery state service subscribed\n");
  }

  // Immediately notify handler with current state
  if (handler != NULL) {
    handler(g_last_battery_state);
  }
}

BatteryChargeState battery_state_service_peek(void) {
  // Return the last known battery state
  return g_last_battery_state;
}

void battery_state_service_unsubscribe(void) {
  BatteryStateServiceState *state = prv_get_state();
  state->handler = NULL;

  // Stop the timer if no subscribers
  if (g_timer_running) {
    k_timer_stop(&g_battery_timer);
    g_timer_running = false;
    printk("Battery state service unsubscribed\n");
  }
}

void battery_state_service_state_init(BatteryStateServiceState *state) {
  *state = (BatteryStateServiceState) {
    .handler = NULL,
  };
}
