/*
 * Copyright (c) 2026 Qingsong Gou <gouqs@hotmail.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "system/pebble_task.h"
#include "kernel/event.h"

static void main_task(void *parameter) {
  prv_main_task_init();
  launcher_main_loop();
}

void launcher_main_loop(void) {
  PBL_LOG(LOG_LEVEL_ALWAYS, "Starting Launcher");

  prv_launcher_main_loop_init();

  while (1) {
    task_watchdog_bit_set(PebbleTask_KernelMain);

    // We make this PebbleEvent static to save stack space
    static PebbleEvent e;
    if (event_take_timeout(&e, 1000)) {
      const PebbleTaskBitset kernel_main_task_bit = (1 << PebbleTask_KernelMain);
      const bool is_not_masked_out_from_kernel_main = !(e.task_mask & kernel_main_task_bit);
      if (is_not_masked_out_from_kernel_main) {
        prv_handle_event(&e);
      }

      event_service_handle_event(&e);

      event_cleanup(&e);

      mcu_fpu_cleanup();
      event_loop_upkeep();
    }
  }

  __builtin_unreachable();
}

void main(void)
{
  struct k_mbox_msg recv_msg;
  printk("Hello Pebble! %s\n");
  int error = 0;

  TaskParameters_t task_params = {
    .pvTaskCode = main_task,
    .pcName = "KernelMain",
    .usStackDepth = kernel_main_stack_words,
    .uxPriority = (tskIDLE_PRIORITY + 3) | portPRIVILEGE_BIT,
    .puxStackBuffer = (void*)(uintptr_t)((uint32_t)__kernel_main_stack_start__
                                          + (uint32_t)__stack_guard_size__)
  };

  pebble_task_create(PebbleTask_KernelMain, &task_params, NULL);
}
