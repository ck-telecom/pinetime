#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#define CHARGER_PIN     DT_GPIO_PIN(DT_ALIAS(charger), gpios)

LOG_MODULE_REGISTER(CHARGER, LOG_LEVEL_INF);

static const struct device *charging_dev;
static struct gpio_callback charging_cb;

static void battery_charging_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    int val = gpio_pin_get(dev, CHARGER_PIN);
    LOG_INF("val = %d", val);
//	battery_update_charging_status(res != 1U);
}

int charger_init(const struct device *dev)
{
    charging_dev = device_get_binding(DT_GPIO_LABEL(DT_ALIAS(charger), gpios));
    if (charging_dev == NULL) {
        return -ENODEV;
    }

    gpio_pin_configure(charging_dev, CHARGER_PIN, GPIO_INPUT | GPIO_INT_EDGE_FALLING | GPIO_PULL_UP | DT_GPIO_FLAGS(DT_ALIAS(charger), gpios));
    gpio_init_callback(&charging_cb, battery_charging_isr, BIT(CHARGER_PIN));
    gpio_add_callback(charging_dev, &charging_cb);
}

SYS_INIT(charger_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);