#include <zephyr.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#include "button.h"
#include "display_api.h"

#define BUTTON_STACK_SIZE 1024
#define BUTTON_PRIORITY 5

#define BTN_DEBOUNCE_TIME 10 /* 10 ms */

static const struct gpio_dt_spec key_in = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});
static const struct gpio_dt_spec key_out = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw1), gpios, {0});

LOG_MODULE_REGISTER(button, LOG_LEVEL_INF);

K_MSGQ_DEFINE(button_msgq, sizeof(struct btn_msg), 10, 4);

static struct gpio_callback button_cb;

static void button_event_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    struct btn_msg m;
    int state = gpio_pin_get_dt(&key_in);
    m.val = (state ? BUTTON_STATE_PRESSED : BUTTON_STATE_RELEASED);
    printk("pin state:%d\n", state);
    k_msgq_put(&button_msgq, &m, K_MSEC(BTN_DEBOUNCE_TIME));
}

static inline int button_is_pressed(uint8_t id)
{
    return gpio_pin_get_dt(&key_in);
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
    //uint32_t now = k_uptime_get();
    //uint32_t diff = now - handle->press_time;

    if (handle->click_config.single_click.up_handler) {
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

int button_init(const struct device *dev)
{
    int retval = 0;

    if (!device_is_ready(key_in.port) || !device_is_ready(key_out.port)) {
        LOG_ERR("device is not ready");
        return -ENODEV;
    }
    /* Set button out pin to high to enable the button */
    gpio_pin_configure_dt(&key_out, GPIO_OUTPUT_ACTIVE);

    gpio_pin_configure_dt(&key_in, GPIO_INPUT);

    gpio_init_callback(&button_cb, button_event_cb, BIT(key_in.pin));
    retval = gpio_add_callback(key_out.port, &button_cb);
    if (retval) {
        return retval;
    }

    retval = gpio_pin_interrupt_configure_dt(&key_in, GPIO_INT_EDGE_BOTH);
    if (retval) {
        LOG_ERR("failed to configure pin %d '%s'\n", key_in.pin, key_in.port->name);
        return retval;
    }

    LOG_INF("button init done");
    return retval;
}
SYS_INIT(button_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void button_thread()
{
    struct btn_msg m;
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
