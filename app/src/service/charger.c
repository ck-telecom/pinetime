#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#include "display_api.h"

#define CHARGER_PIN     DT_GPIO_PIN(DT_ALIAS(charger), gpios)

LOG_MODULE_REGISTER(CHARGER, LOG_LEVEL_INF);

static const struct gpio_dt_spec charger = GPIO_DT_SPEC_GET_OR(DT_ALIAS(charger), gpios, {0});
static struct gpio_callback charger_cb;

static void charger_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    struct msg msg;
    int val = gpio_pin_get(dev, CHARGER_PIN);
    LOG_INF("val = %d", val); // 1: plug-in, 0: unplug
    msg_send_event(&msg, MSG_TYPE_CHARGER, val);
}

int charger_init(const struct device *dev)
{
    int retval = 0;

    if (!device_is_ready(charger.port)) {
		LOG_ERR("button device %s is not ready\n", charger.port->name);
		return -ENODEV;
    }

    retval = gpio_pin_configure_dt(&charger, GPIO_INPUT);
    if (retval)
        return retval;

    retval = gpio_pin_interrupt_configure_dt(&charger, GPIO_INT_EDGE_BOTH);
    if (retval)
        return retval;

    gpio_init_callback(&charger_cb, charger_isr, BIT(charger.pin));
    retval = gpio_add_callback(charger.port, &charger_cb);

    return retval;
}

SYS_INIT(charger_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
