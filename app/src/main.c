/*
 * Copyright (c) 2026 Qingsong Gou <gouqs@hotmail.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    printk("Hello Zephyr on PineTime!\n");

    while (1) {
        k_msleep(1000);
        printk("Main thread: KernelMain is running...\n");
    }

    return 0;
}
