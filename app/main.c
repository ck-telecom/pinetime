/*
 * Copyright (c) 2026 Qingsong Gou <gouqs@hotmail.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "system/pebble_task.h"

static void main_task(void *parameter) {
  prv_main_task_init();
  launcher_main_loop();
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
