#include <zephyr.h>
#include <drivers/gpio.h>


#define BTN_PORT    DT_GPIO_LABEL(DT_ALIAS(sw0), gpios)

#define KEY_IN      DT_GPIO_PIN(DT_ALIAS(sw0), gpios)
#define KEY_OUT     DT_GPIO_PIN(DT_ALIAS(sw1), gpios)

#define BTN_DEBOUNCE_TIME 200 /* 200 ms */

static struct k_timer timer_debounce;


// K_MSGQ_DEFINE(evt_msgq, sizeof(struct msg), 10, 4);

//static sys_slist_t evt_list;
static struct gpio_callback button_cb;

struct node {
    int (*func)(void *arg);
    //struct sys_slist_t evt_list;
};

void button_event_send(struct k_timer *timer)
{
    // send btn event
    // k_msg_put();
}

static void button_event_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    k_timer_start(&timer_debounce, K_NO_WAIT, K_MSEC(BTN_DEBOUNCE_TIME));
}

int btn_event_init(const struct device *dev)
{
    int retval = 0;
    k_timer_init(&timer_debounce, button_event_send, NULL);

    const struct device *button_dev = device_get_binding(BTN_PORT);
    if (!button_dev) {
        return -ENODEV;
    }

    /* Set button out pin to high to enable the button */
    gpio_pin_configure(button_dev, KEY_OUT, GPIO_OUTPUT);
    gpio_pin_set_raw(button_dev, KEY_OUT, 1U);

    gpio_pin_configure(button_dev, KEY_IN, GPIO_INPUT | GPIO_INT_EDGE_FALLING/* | PULL_UP*/);

    gpio_init_callback(&button_cb, button_event_cb, BIT(KEY_IN));
    gpio_add_callback(button_dev, &button_cb);

    return retval;
}

SYS_INIT(btn_event_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);