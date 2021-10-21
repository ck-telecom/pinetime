#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#define CHARGER_PIN     DT_GPIO_PIN(DT_ALIAS(charger), gpios)

LOG_MODULE_REGISTER(CHARGER, LOG_LEVEL_INF);

static const struct device *charger_dev;
static struct gpio_callback charging_cb;

static void battery_charging_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    int val = gpio_pin_get(dev, CHARGER_PIN);
    LOG_INF("val = %d", val);
//	battery_update_charging_status(res != 1U);
}

int charger_init(const struct device *dev)
{
    charger_dev = device_get_binding(DT_GPIO_LABEL(DT_ALIAS(charger), gpios));
    if (charger_dev == NULL) {
        return -ENODEV;
    }

    gpio_pin_interrupt_configure(charger_dev, CHARGER_PIN, GPIO_INT_EDGE_BOTH);
    gpio_init_callback(&charger_dev, battery_charging_isr, BIT(CHARGER_PIN));
    gpio_add_callback(charger_dev, &charging_cb);
}

SYS_INIT(charger_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);