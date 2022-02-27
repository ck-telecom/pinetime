#ifndef _VIBES_H
#define _VIBES_H

struct vibe_pattern {
	const uint32_t *durations;
	uint32_t num_segments;
};

void vibes_cancel(void);

void vibes_short_pulse(void);

void vibes_long_pulse(void);

void vibes_double_pulse(void);

void vibes_enqueue_custom_pattern(const struct vibe_pattern *pattern);

#endif /* _VIBES_H */