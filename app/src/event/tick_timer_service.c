#include <drivers/counter.h>

#define SECOND_DELAY 1000000
#define MINUTE_DELAY (1000000 * 60)

#define ALARM_CHANNEL_ID 0

typedef struct TickTimerState {
    CoreTimer timer; /* must be at the start of the struct! */
    int onqueue;
	TimeUnits units;
	TickHandler handler;
	struct counter_alarm_cfg alarm_cfg;
} TickTimerState;

/* TODO: this should probably be per-app.  oh, well */
static TickTimerState _state = {0};

const struct device *counter_dev;
int counter_init()
{
	struct counter_alarm_cfg alarm_cfg;


	if (counter_dev == NULL) {
		LOG_ERR("Device not found\n");
		return -ENODEV;
	}
	counter_start(counter_dev);
}

static void counter_interrupt_cb(const struct device *counter_dev,
				 uint8_t chan_id, uint32_t ticks,
				 void *user_data)
{
	struct counter_alarm_cfg *config = user_data;
        TickTimerState *state = CONTAINER_OF(config, TickTimerState, alarm_cfg);
        uint32_t now_ticks;
        uint64_t now_usec;
        int now_sec;
        int err;

        err = counter_get_value(counter_dev, &now_ticks);
        if (err) {
                printk("Failed to read counter value (err %d)", err);
                return;
        }

        now_usec = counter_ticks_to_us(counter_dev, now_ticks);
        now_sec = (int)(now_usec / USEC_PER_SEC);

        printk("!!! Alarm !!!\n");
        printk("Now: %u\n", now_sec);

        printk("Set alarm in %u sec (%u ticks)\n",
               (uint32_t)(counter_ticks_to_us(counter_dev,
                                           config->ticks) / USEC_PER_SEC),
               config->ticks);

        err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID,
                                        user_data);
        if (err != 0) {
                printk("Alarm could not be set\n");
        }
}

void tick_timer_service_subscribe(TimeUnits tick_units, TickHandler handler)
{
	TickTimerState *state = &_state;
	int err;

	if (tick_units & SECOND_UNIT) {
		state->alarm_cfg.flags = 0;
		state->alarm_cfg.ticks = counter_us_to_ticks(counter_dev, SECOND_DELAY);
		state->alarm_cfg.callback = counter_interrupt_cb;
		state->alarm_cfg.user_data = &alarm_cfg;
	} else if (tick_units & MINUTE_UNIT) {
		state->alarm_cfg.flags = 0;
		state->alarm_cfg.ticks = counter_us_to_ticks(counter_dev, MINUTE_DELAY);
		state->alarm_cfg.callback = counter_interrupt_cb;
		state->alarm_cfg.user_data = &alarm_cfg;
	} else {

	}
	state->units = tick_units;
	state->handler = handler;
	err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID, &state->alarm_cfg);
	if (err) {

	}
}
