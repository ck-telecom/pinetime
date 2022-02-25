#ifndef _VIBES_H
#define _VIBES_H

typedef struct vibe_pattern {
	const uint32_t *durations;
	uint32_t num_segments;
} VibePattern;

void vibes_cancel(void);

void vibes_short_pulse(void);

void vibes_long_pulse(void);

void vibes_double_pulse(void);

void vibes_enqueue_custom_pattern(VibePattern pattern);

#endif /* _VIBES_H */