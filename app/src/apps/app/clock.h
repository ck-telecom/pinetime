#ifndef _CLOCK_H
#define _CLOCK_H

#include <lvgl.h>

#include "../app.h"

typedef struct clock_app {
	app_t app;
	lv_obj_t *screen;

	lv_style_t style_line;
	lv_style_t hour_line_style;
	lv_style_t minute_line_style;
	lv_style_t second_line_style;

	lv_obj_t *hour_body;
	lv_obj_t *minute_body;
	lv_obj_t *second_body;

	lv_point_t hour_point[2];
	lv_point_t minute_point[2];
	lv_point_t second_point[2];

	lv_timer_t *lv_timer_clock;
} clock_app_t;

#define HOUR_LENGTH     70
#define MINUTE_LENGTH   90
#define SECOND_LENGTH   110

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)  ( sizeof(a) / sizeof((a)[0]) )
#endif

#define LV_TRIG_SCALE   lv_trigo_sin(90)

struct gua {
	lv_point_t line_point[2];
};

extern clock_app_t clock_app;
#define APP_CLOCK (&clock_app.app)

#endif /* _CLOCK_H */
