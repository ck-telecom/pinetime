/*
 * Copyright (c) 2026 Qingsong Gou <gouqs@hotmail.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "kernel/events.h"
#include "kernel/event_loop.h"

#define KERNEL_MAIN_STACK_SIZE 4096

K_THREAD_STACK_DEFINE(kernel_main_stack, KERNEL_MAIN_STACK_SIZE);
static struct k_thread kernel_main_thread;
static k_tid_t kernel_main_thread_id;

static void launcher_main_loop_wrapper(void *p1, void *p2, void *p3)
{
    printk("Launcher main loop wrapper called\n");
    launcher_main_loop();
}

int main(void)
{
  printk("Hello Zephyr on PineTime!\n");
  printk("Initializing PebbleOS KernelMain task...\n");

  //events_init();
  //tick_init();
  //app_outbox_service_init();

  kernel_main_thread_id = k_thread_create(&kernel_main_thread, kernel_main_stack,
                                          K_THREAD_STACK_SIZEOF(kernel_main_stack),
                                          launcher_main_loop_wrapper, NULL, NULL, NULL,
                                          1, 0, K_NO_WAIT);

  printk("PebbleOS KernelMain task initialized with thread ID: %p\n", kernel_main_thread_id);

  while (1) {
      k_msleep(1000);
      printk("Main thread: KernelMain is running...\n");
  }

  return 0;
}
