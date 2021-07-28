#include <zephyr.h>

#include "vibrator.h"

#define VIRBRATOR_STACK_SIZE    1024
#define VIBRATOR_PRIORITY       6


K_MSGQ_DEFINE(vibrator_q, sizeof(const struct vibrate_pattern *), 8, 4);


static const struct vibrate_pattern default_vibrate_patterns[VIBRATOR_CMD_MAX] = {
    // VIBRATE_CMD_PLAY_PATTERN_1
    {
        .length = 1,
        .buffer = (const struct vibrate_pattern_pair []) {
            { .frequency = 255, .duration_ms = 1000 }
        },
    },

    // VIBRATE_CMD_PLAY_PATTERN_2
    {
        .length = 2,
        .buffer = (const struct vibrate_pattern_pair []) {
            { .frequency = 255, .duration_ms = 100 },
            { .frequency = 255, .duration_ms = 750 }
        },
    },

    // VIBRATE_CMD_PLAY_PATTERN_3
    {
        .length = 25,
        .buffer = (const struct vibrate_pattern_pair []) {
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100},
            {.frequency = 127, .duration_ms = 750},
            {.frequency = 255, .duration_ms = 100}
        },
    },

    // VIBRATE_CMD_STOP
    {
        .length =  0,
        .buffer = (const struct vibrate_pattern_pair []) {
            {.frequency = 0, .duration_ms = 0}
        }
    }
};

static int virbrator_send_command(const struct vibrate_pattern *pattern)
{
    const struct vibrate_pattern *p = pattern;
    return k_msgq_put(&vibrator_q, &p, K_FOREVER);
}

void virbrator_command(enum vibrator_cmd cmd)
{
    if (cmd < VIBRATOR_CMD_MAX) {
        virbrator_send_command(&default_vibrate_patterns[cmd]);
    } else {
    }
}

void vibrator_play_pattern(const struct vibrate_pattern *pattern)
{

}
/*
virbrator_enable()
virbrator_disable()
*/
static void virbrator_thread()
{
    uint8_t buf_idx = 0;
    uint8_t idx = 0;
    static const struct vibrate_pattern *current_pattern = &default_vibrate_patterns[VIBRATE_CMD_STOP];
    for (; ;) {
        if (k_msgq_get(&vibrator_q, &current_pattern, K_FOREVER))
            continue;

        buf_idx = 0;
        while (buf_idx < current_pattern->length) {
        // vibrator_hw_set_frequency();
            k_sleep(K_MSEC(current_pattern->buffer[idx].duration_ms));
            if (k_msgq_get(&vibrator_q, &current_pattern, K_NO_WAIT)) {
                /* if we just recieved another pattern (including stop), start playing it from the beginning */
                buf_idx = 0;
            } else {
                buf_idx++;
            }
        }
        current_pattern = &default_vibrate_patterns[VIBRATE_CMD_STOP];
    }
}
K_THREAD_DEFINE(virbrator, VIRBRATOR_STACK_SIZE, virbrator_thread,
                NULL, NULL, NULL,
                VIBRATOR_PRIORITY, 0, 0);