#include <zephyr.h>

#define CHARGER_PIN     12

static const struct device *charging_dev;
static struct gpio_callback charging_cb;

static void battery_charging_isr(const struct device *gpiobat, struct gpio_callback *cb, uint32_t pins)
{
//    uint32_t res = gpio_pin_get(charging_dev, BAT_CHA);
//	battery_update_charging_status(res != 1U);
}

int charger_init(const struct device *dev)
{
    charging_dev = device_get_binding("GPIO_0");
    if (charging_dev ==NULL) {
        return -ENODEV;
    }

    gpio_pin_configure(charging_dev, CHARGER_PIN, GPIO_INPUT | GPIO_INT_EDGE_BOTH);
    gpio_init_callback(&charging_cb, battery_charging_isr, BIT(BAT_CHA));
    gpio_add_callback(charging_dev, &charging_cb);
}

SYS_INIT(charger_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);