#include "backlight.h"

static const struct device* backlight_dev;

#define BACKLIGHT_PORT  DT_GPIO_LABEL(DT_ALIAS(led1), gpios)

#define BACKLIGHT_1     DT_GPIO_PIN(DT_ALIAS(led0), gpios)
#define BACKLIGHT_2     DT_GPIO_PIN(DT_ALIAS(led1), gpios)
#define BACKLIGHT_3     DT_GPIO_PIN(DT_ALIAS(led2), gpios)

int backlight_init()
{
    backlight_dev = device_get_binding(BACKLIGHT_PORT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_1, GPIO_OUTPUT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_2, GPIO_OUTPUT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_3, GPIO_OUTPUT);
    backlight_enable(true);
}

int backlight_enable(bool enable)
{
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_1, enable ? 0 : 1);
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_2, enable ? 0 : 1);
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_3, enable ? 0 : 1);
}