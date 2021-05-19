/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

void main(void)
{
//	printk("Hello World! %s\n", CONFIG_BOARD);


    //lv_task_handler();

    while (1)
	{
        //lv_task_handler();
        k_sleep(K_MSEC(10));
	}
}
