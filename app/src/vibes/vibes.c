#include <zephyr.h>
#include <drivers/gpio.h>

#include "vibes.h"

#define VIBES_STACK_SIZE 1024
#define VIBRAES_PRIORITY  6 

/* The devicetree node identifier for the "led0" alias. */
#define VIBE_NODE DT_ALIAS(vibe)

static const struct gpio_dt_spec vibe = GPIO_DT_SPEC_GET(VIBE_NODE, gpios);

K_MSGQ_DEFINE(vibes_q, sizeof(struct vibe_pattern *), 8, 4);

static const struct vibe_pattern pattern_short = {
	.num_segments = 1,
	.durations = (const uint32_t []) { 10 }
};

static const struct vibe_pattern pattern_long = {
	.num_segments = 1,
	.durations = (const uint32_t []) { 100 }
};

static const struct vibe_pattern pattern_double = {
	.num_segments = 3,
	.durations = (const uint32_t []) { 50, 50, 50 }
};

void vibes_cancel(void)
{
	k_msgq_purge(&vibes_q);
}

void vibes_short_pulse(void)
{
	const struct vibe_pattern *p = &pattern_short;

	k_msgq_put(&vibes_q, p, K_FOREVER);
}

void vibes_long_pulse(void)
{
	const struct vibe_pattern *p = &pattern_long;

	k_msgq_put(&vibes_q, p, K_FOREVER);
}

void vibes_double_pulse(void)
{
	const struct vibe_pattern *p = &pattern_double;

	k_msgq_put(&vibes_q, p, K_FOREVER);
}

void vibes_enqueue_custom_pattern(const struct vibe_pattern *pattern)
{
	const struct vibe_pattern *p = pattern;

	k_msgq_put(&vibes_q, p, K_FOREVER);
}

static void vibes_enable(bool enabled)
{
	int val = enabled ? 1 : 0;

	gpio_pin_set_dt(&vibe, val);
}

static void vibes_thread()
{
	uint8_t idx = 0;
	int ret;

	if (!device_is_ready(vibe.port)) {
		return;
	}

	ret = gpio_pin_configure_dt(&vibe, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return;
	}

	struct vibe_pattern *curr_pattern;
	printk("vibrator init done\n");
	for (; ;) {
		if (k_msgq_get(&vibes_q, &curr_pattern, K_FOREVER)) {
			continue;
		}
		idx = 0;

		while (idx < curr_pattern->num_segments) {
			bool enabled = (idx % 2 == 0) ? true : false;
			vibes_enable(enabled);
			k_sleep(K_MSEC(curr_pattern->durations[idx]));
			vibes_enable(false);
			if (k_msgq_get(&vibes_q, &curr_pattern, K_NO_WAIT)) {
				idx = 0;
			} else {
				idx++;
			}
		}
	}
}
K_THREAD_DEFINE(vibes_tid, VIBES_STACK_SIZE, vibes_thread,
				NULL, NULL, NULL,
				VIBRAES_PRIORITY, 0, 0);