#ifndef _BUTTON_H
#define _BUTTON_H

#include <stdint.h>

#define NUM_BUTTONS     1

enum {
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_REPEATING,
    BUTTON_STATE_LONG,
    BUTTON_STATE_RELEASED
};



typedef void (*click_handler)(void *recognizer, void *context);

struct click_config {
    struct {
        click_handler up_handler;
        click_handler down_handler;
    } single_click;

    struct {
        uint32_t delay_ms;
        click_handler handler;
        click_handler release_handler;
    } long_click;

    struct {
        click_handler handler;
        uint32_t repeat_interval_ms;
    } click;
};

struct button_handle {
//    const *struct devices dev;
//    port;
//    struct gpio_callback button_cb;
//    uint32_t timestamp;
    uint32_t repeat_time;
	uint32_t press_time;
	uint32_t state;
	struct click_config click_config;
};

typedef struct button_handle button_handle_t;

struct msg {
	uint32_t val;	
};

extern struct button_handle button_handle_array[];

#endif