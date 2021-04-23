#include <zephyr.h>
#include <drivers/gpio.h

static struct k_timer timer_debounce;

void button_event_send(struct k_timer *timer)
{
    // send btn event
        k_msg_put();
}

static void button_pressed_cb(struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    k_timer_start(&timer_debounce, K_NO_WAIT, K_SECONDS(BTN_DEBOUNCE_TIME));
}

int btn_event_init()
{
    int retval = 0;
    k_timer_init(&timer_debounce, button_event_send, NULL);

    struct device *button_dev = device_get_binding("gpio");
    if (!button_dev) {
        return -1;
    }

    gpio_pin_configure(button_dev, BTN_IN, GPIO_INPUT | GPIO_INT_EDGE_FALLING | PULL_UP);

    gpio_init_callback(&button_cb, button_pressed_cb, BIT(BTN_IN));
    gpio_add_callback(button_dev, &button_cb);

    return retval;
}