#if 0
#include <stdbool.h>
#include <drivers/gpio.h>

#include "backlight.h"

static const struct device* backlight_dev;

#define BACKLIGHT_PORT  DT_GPIO_LABEL(DT_ALIAS(led1), gpios)

#define BACKLIGHT_1     DT_GPIO_PIN(DT_ALIAS(led0), gpios)
#define BACKLIGHT_2     DT_GPIO_PIN(DT_ALIAS(led1), gpios)
#define BACKLIGHT_3     DT_GPIO_PIN(DT_ALIAS(led2), gpios)

int backlight_init(const struct device *dev)
{
    backlight_dev = device_get_binding(BACKLIGHT_PORT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_1, GPIO_OUTPUT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_2, GPIO_OUTPUT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_3, GPIO_OUTPUT);
    backlight_enable(true);

    return 0;
}

int backlight_enable(bool enable)
{
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_1, enable ? 0 : 1);
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_2, enable ? 0 : 1);
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_3, enable ? 0 : 1);

    return 0;
}
#else
#include <drivers/led.h>

#include "backlight.h"

static const struct device* backlight_dev;

#if DT_NODE_HAS_STATUS(DT_INST(0, pwm_leds), okay)
#define LED_PWM_NODE_ID		DT_INST(0, pwm_leds)
#define LED_PWM_DEV_NAME	DEVICE_DT_NAME(LED_PWM_NODE_ID)
#else
#error "No LED PWM device found"
#endif

static int backlight_init(const struct device *dev)
{
    backlight_dev = device_get_binding(LED_PWM_DEV_NAME);

    led_set_brightness(backlight_dev, 0, 100);

    return 0;
}

int backlight_enable(bool enable)
{
    if (enable) {
        return led_on(backlight_dev, 0);
    } else {
        return led_off(backlight_dev, 0);
    }
}

int backlight_set_brighness(uint8_t value)
{
    return led_set_brightness(backlight_dev, 0, value);
}
#endif

SYS_INIT(backlight_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
