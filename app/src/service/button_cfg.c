#include "button.h"
#include "display.h"

static void handler(void *p, void *p1)
{

}

static void long_click_handler(void *p, void *p1)
{
    struct msg msg;
    msg_send_button(&msg, BUTTON_STATE_LONG);
}

static void single_click_down_handler(void *p, void *p1)
{
    struct msg msg;
    msg_send_button(&msg, BUTTON_STATE_PRESSED);
}

static void single_click_up_handler(void *p, void *p1)
{
    struct msg msg;
    msg_send_button(&msg, BUTTON_STATE_RELEASED);
}

struct button_handle button_handle_array[NUM_BUTTONS] = {
    [0] = {
        .repeat_time = 0,
        .press_time = 0,
        .click_config = {
            .single_click = {
                .up_handler = single_click_up_handler,
                .down_handler = single_click_down_handler,
            },
            .long_click = {
                .delay_ms = 50,
                .handler = long_click_handler,
                .release_handler = handler,
            },
            .click = {
                .handler = handler,
                .repeat_interval_ms = 50,
            }
        },
    },
};