#ifndef _VIBRATOR_H
#define _VIBRATOR_H

#include <stdint.h>

enum vibrator_cmd {
    VIBRATOR_CMD_PATTEN_1,
    VIBRATOR_CMD_PATTEN_2,
    VIBRATOR_CMD_PATTEN_3,
    VIBRATOR_CMD_PATTEN_4,
    VIBRATE_CMD_STOP,
    VIBRATOR_CMD_MAX
};

struct vibrate_pattern_pair {
    uint16_t frequency;
    uint16_t duration_ms;
};

struct vibrate_pattern {
    const uint8_t length;
    const struct vibrate_pattern_pair * const buffer;
};

void vibrator_command(enum vibrator_cmd cmd);

#endif