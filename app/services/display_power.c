/*
 * Copyright (c) 2021 gouqs@hotmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <drivers/gpio.h>
#include <drivers/display.h>
#include <logging/log.h>
#include <pm/device.h>

#include "display_power.h"

static const struct device *display_dev;


void display_power_init(void)
{
    display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
    display_blanking_off(display_dev);
}

void display_power_sleep(void)
{
    (void)pm_device_state_set(display_dev, PM_DEVICE_ACTION_SUSPEND);
}

void display_power_wakeup(void)
{
    (void)pm_device_state_set(display_dev, PM_DEVICE_ACTION_RESUME);
}