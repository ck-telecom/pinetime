#include <zephyr.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#include "button.h"

#define BUTTON_STACK_SIZE 1024
#define BUTTON_PRIORITY 5

#define BTN_PORT    DT_GPIO_LABEL(DT_ALIAS(led0), gpios)

#define KEY_IN      DT_GPIO_PIN(DT_ALIAS(sw0), gpios)
#define KEY_OUT     DT_GPIO_PIN(DT_ALIAS(sw1), gpios)

#define BTN_DEBOUNCE_TIME 100 /* 100 ms */


LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);

static const struct device *button_dev;
static struct k_timer timer_debounce;

static void button_event_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    /* start one shot timer that expires after 200 ms */
    k_timer_start(&timer_debounce, K_MSEC(BTN_DEBOUNCE_TIME), K_NO_WAIT);
}
K_MSGQ_DEFINE(button_msgq, sizeof(struct msg), 10, 4);

//static sys_slist_t evt_list;
static struct gpio_callback button_cb;


struct node {
    int (*func)(void *arg);
    //struct sys_slist_t evt_list;
};

static inline int button_is_pressed(uint8_t id)
{
    return gpio_pin_get(button_dev, KEY_IN);
}

static void button_pressed(struct button_handle *handle)
{
    handle->press_time = k_uptime_get();
    handle->repeat_time = handle->press_time;
    if (handle->click_config.single_click.down_handler) {
        LOG_INF("button pressed");
    }

    handle->state = BUTTON_STATE_PRESSED;
}

static void button_released(struct button_handle *handle)
{
    uint32_t now = k_uptime_get();
    uint32_t diff = now - handle->press_time;
    /* check long press timer expired */
    if (handle->click_config.long_click.handler && handle->state == BUTTON_STATE_LONG) {
        LOG_INF("button long released");
    } else if (handle->click_config.single_click.up_handler) {
        LOG_INF("button released");
    }
    handle->repeat_time = 0;
    handle->press_time = 0;
    handle->state = BUTTON_STATE_RELEASED;
}

static void button_update(uint8_t id, uint8_t pressed)
{
    struct button_handle *handle = &button_handle_array[id];

    if (pressed == BUTTON_STATE_PRESSED) {
        button_pressed(handle);
    } else {
        button_released(handle);
    }
}

static uint8_t button_check_time()
{
    struct button_handle *button = NULL;
    uint8_t pressed = 0;
    uint8_t any_pressed = 0;
    uint32_t now = k_uptime_get();
    for (int i = 0; i < NUM_BUTTONS; i++) {
        button = &button_handle_array[i];
        pressed = button_is_pressed(i);
        if (!pressed) {
            continue;
        } else {
            any_pressed = 1;
        }
        /* button is still pressed */
/*        if (button->click_config.single_click.handler && button->click_config.single_click.repeat_interval_ms) {
            if (now > button->repeat_time + button->click_config.single_click.repeat_interval_ms) {
                button->state = BUTTON_STATE_REPEATING;

                button->repeat_time = now;
            }
        }*/
        /* button pressed */
        if (button->click_config.long_click.handler && button->press_time > 0 &&
            button->click_config.long_click.delay_ms > 0) {
            if (now > button->press_time + button->click_config.long_click.delay_ms) {
                button->state = BUTTON_STATE_LONG;
                LOG_INF("button long pressed");
                button->press_time = 0;
                button->repeat_time = 0;
            }
        }
    }

    return any_pressed;
}

void button_event_send(struct k_timer *timer)
{
    const struct device *button_dev = k_timer_user_data_get(timer);

    int state = gpio_pin_get(button_dev, KEY_IN);
    uint32_t val = (state ? BUTTON_STATE_PRESSED : BUTTON_STATE_RELEASED);
    printk("pin state:%d\n", state);

    k_msgq_put(&button_msgq, &val, K_NO_WAIT);
}

int button_init(const struct device *dev)
{
    int retval = 0;

    button_dev = device_get_binding(BTN_PORT);
    if (!button_dev) {
        return -ENODEV;
    }

    k_timer_init(&timer_debounce, button_event_send, NULL);
    k_timer_user_data_set(&timer_debounce, (void *)button_dev);

    /* Set button out pin to high to enable the button */
    gpio_pin_configure(button_dev, KEY_OUT, GPIO_OUTPUT_ACTIVE | DT_GPIO_FLAGS(DT_ALIAS(sw1), gpios));

    retval = gpio_pin_configure(button_dev, KEY_IN, GPIO_INPUT | GPIO_INT_EDGE_BOTH | DT_GPIO_FLAGS(DT_ALIAS(sw0), gpios));
    if (retval) {
        LOG_ERR("failed to configure pin %d '%s'\n", KEY_IN, DT_LABEL(DT_ALIAS(sw0)));
    }

    gpio_init_callback(&button_cb, button_event_cb, BIT(KEY_IN));
    gpio_add_callback(button_dev, &button_cb);
#if 0
    for (int i = 0; i < NUM_BUTTONS; i++) {
        handle[i].dev = device_get_binding(handle[i].port);
        gpio_pin_configure(handle[i].dev, handle[i].pin, GPIO_INPUT | GPIO_INT_EDGE_BOTH/* | PULL_UP*/);
        gpio_init_callback(&handle[i].button_cb, button_event_cb, BIT(handle[i].pin));
        gpio_add_callback(&handle[i].dev, &handle[i].button_cb);
    }
#endif
    LOG_INF("button init done");
    return retval;
}
SYS_INIT(button_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

static void button_thread()
{
    struct msg m;
    uint8_t flag = 0;
    k_timeout_t timeout = K_FOREVER;
    for (; ;) {
        if (k_msgq_get(&button_msgq, &m, timeout) == 0) {
            button_update(0, /* button_is_pressed(id) */m.val);
        }

        flag = button_check_time();
        timeout = ( flag == 0 ? K_FOREVER : K_MSEC(10) );
        // LOG_INF("check time");
    }
}
K_THREAD_DEFINE(button_tid, BUTTON_STACK_SIZE, button_thread, NULL, NULL, NULL, BUTTON_PRIORITY, 0, 0);
