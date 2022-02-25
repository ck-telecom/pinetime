#include <zephyr.h>

#include "vibes.h"

#define VIBRES_STACK_SIZE 1024
#define VIBRAES_PRIORITY  6 

K_MSGQ_DEFINE(vibres_q, sizeof(VibePattern), 8, 4);

void vibes_cancel(void)
{
}

void vibes_short_pulse(void)
{
}

void vibes_long_pulse(void)
{
}

void vibes_double_pulse(void)
{
}

void vibes_enqueue_custom_pattern(VibePattern pattern)
{
}

static void vibres_enable(bool enabled)
{
	
}

static void vibres_thread()
{
	uint8_t idx = 0;

	pwm_dev = DEVICE_DT_GET(PWM_CTLR);
	if (!device_is_ready(pwm_dev)) {
		printk("Error: didn't find %s device\n", pwm_dev->name);
		return;
	}

	VibePattern curr_pattern;
	printk("vibrator init done\n");
	for (; ;) {
		if (k_msgq_get(&vibres_q, &curr_pattern, K_FOREVER)) {
			continue;
		}
		idx = 0;

		while (idx < curr_pattern->num_segments) {
			bool enabled = (idx % 2 == 0) ? true : false;
			vibres_enable(enabled);
			k_sleep(K_MSEC(curr_pattern->durations[idx]));
			if (k_msgq_get(&vibres_q, &curr_pattern, K_NO_WAIT)) {
				idx = 0;
			} else {
				idx++;
			}
		}
	}
}
K_THREAD_DEFINE(vibres_thread, VIBRES_STACK_SIZE, vibres_thread,
				NULL, NULL, NULL,
				VIBRAES_PRIORITY, 0, 0);