#include "button.h"

static void handler(void *p, void *p1)
{
    
}

struct button_handle button_handle_array[NUM_BUTTONS] = {
    [0] = {
        .repeat_time = 0,
        .press_time = 0,
        .click_config = {
            .single_click = {
                .up_handler = handler,
                .down_handler = handler,
            },
            .long_click = {
                .delay_ms = 50,
                .handler = handler,
                .release_handler = handler,
            },
            .click = {
                .handler = handler,
                .repeat_interval_ms = 50,
            }
        },
    },
};