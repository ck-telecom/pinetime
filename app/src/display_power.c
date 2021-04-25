/*
 * Copyright (c) 2021 gouqs@hotmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <drivers/gpio.h>
#include <drivers/display.h>
#include <display.h>
#include "display.h"
#include "log.h"


static const struct device *display_dev;


void display_power_init(void)
{
    display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
    display_blanking_off(display_dev);
}

void display_power_sleep(void)
{
    (void)device_set_power_state(display_dev, DEVICE_PM_LOW_POWER_STATE, NULL, NULL);
}

void display_power_wakeup(void)
{
    (void)device_set_power_state(display_dev, DEVICE_PM_ACTIVE_STATE, NULL, NULL);
}