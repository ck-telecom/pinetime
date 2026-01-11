/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "kernel/event_loop.h"
#include "kernel/events.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

//#include "applib/app_launch_reason.h"
//#include "applib/battery_state_service.h"
//#include "applib/connection_service.h"
//#include "applib/graphics/graphics.h"
//#include "applib/graphics/text.h"
//#include "applib/tick_timer_service.h"
//#include "applib/ui/animation_private.h"
//#include "applib/ui/app_window_click_glue.h"
//#include "applib/ui/ui.h"
//#include "applib/ui/window.h"
//#include "applib/ui/window_private.h"
//#include "comm/ble/kernel_le_client/kernel_le_client.h"
//#include "console/serial_console.h"
//#include "console/prompt.h"
//#include "drivers/backlight.h"
//#include "drivers/battery.h"
//#include "drivers/button.h"
//#include "drivers/task_watchdog.h"
//#include "kernel/core_dump.h"
//#include "kernel/kernel_applib_state.h"
//#include "kernel/low_power.h"
//#include "kernel/panic.h"
//#include "kernel/pbl_malloc.h"
//#include "kernel/ui/kernel_ui.h"
//#include "kernel/ui/modals/modal_manager.h"
//#include "kernel/util/factory_reset.h"
//#include "mcu/fpu.h"
//#include "pebble_errors.h"
//#include "process_management/app_install_manager.h"
//#include "process_management/app_manager.h"
//#include "process_management/app_run_state.h"
//#include "process_management/process_manager.h"
//#include "process_management/worker_manager.h"
//#include "resource/resource_ids.auto.h"
//#include "services/common/analytics/analytics.h"
//#include "services/common/battery/battery_state.h"
//#include "services/common/battery/battery_monitor.h"
//#include "services/common/compositor/compositor.h"
//#include "services/common/cron.h"
//#include "services/common/debounced_connection_service.h"
//#include "services/common/ecompass.h"
//#include "services/common/event_service.h"
//#include "services/common/evented_timer.h"
//#include "services/common/firmware_update.h"
//#include "services/common/i18n/i18n.h"
//#include "services/common/light.h"
//#include "services/common/new_timer/new_timer.h"
//#include "services/common/put_bytes/put_bytes.h"
//#include "services/common/status_led.h"
//#include "services/common/system_task.h"
//#include "services/common/vibe_pattern.h"
//#include "services/normal/accessory/accessory_manager.h"
//#include "services/normal/alarms/alarm.h"
//#include "services/normal/app_fetch_endpoint.h"
//#include "services/normal/blob_db/api.h"
//#include "services/normal/notifications/do_not_disturb.h"
//#include "services/normal/stationary.h"
//#include "services/normal/timeline/reminders.h"
//#include "services/normal/wakeup.h"
//#include "services/runlevel.h"
//#include "shell/normal/app_idle_timeout.h"
//#include "shell/normal/watchface.h"
//#include "shell/prefs.h"
//#include "shell/shell_event_loop.h"
//#include "shell/system_app_state_machine.h"
//#include "system/bootbits.h"
#include "system/logging.h"
//#include "system/passert.h"
//#include "system/reset.h"
//#include "system/testinfra.h"
//#include "util/bitset.h"
//#include "util/struct.h"
//#include "system/version.h"

//#include <bluetooth/reconnect.h>
//
//#include "FreeRTOS.h"
//#include "task.h"

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

}

bool launcher_popups_are_blocked(void) {

}

void launcher_cancel_force_quit(void) {

}

void launcher_main_loop(void) {
  //PBL_LOG(LOG_LEVEL_ALWAYS, "Starting Launcher");

  while (1) {
    // We make this PebbleEvent static to save stack space
    static PebbleEvent e;
    if (event_take_timeout(&e, 1000)) {
    }
  }
  __builtin_unreachable();
}
