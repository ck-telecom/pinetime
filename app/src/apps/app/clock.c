#include <zephyr.h>
#include <lvgl.h>
#include <stdlib.h>
#include <time.h>

#include "clock.h"

static const app_spec_t clock_spec;

clock_app_t clock_app = {
	.app = { .spec = &clock_spec }
};

static inline clock_app_t *_from_app(app_t *app)
{
	return CONTAINER_OF(app, clock_app_t, app);
}

struct gua gua_lines[] = {
	{
		{ { 59, 0 }, { 59, 5} } // 11
	},
	{
		{ { 119, 0 }, { 119, 5} } // 12
	},
	{
		{ { 179, 0 }, { 179, 5} } // 1
	},
	{
		{ { 234, 59 }, { 239, 59} } // 2
	},
	{
		{ { 119, 234 }, { 119, 239 } } // 6
	},
	{
		{ { 0, 119 }, { 5, 119} } // 9
	},
	{
		{ { 234, 119 }, { 239, 119} } // 3
	},
	{
		{ { 234, 179 }, { 239, 179} } // 4
	},
	{
		{ { 179, 234 }, { 179, 239} } // 5
	},
	{
		{ { 59, 234 }, { 59, 239} } // 7
	},
	{
		{ { 0, 179 }, { 5, 179} } // 8
	},
	{
		{ { 0, 59 }, { 5, 59} } // 10
	},
};

 static int16_t Cosine(int16_t angle)
{
	return _lv_trigo_sin(angle + 90);
}

static int16_t Sine(int16_t angle)
{
	return _lv_trigo_sin(angle);
}

static int16_t coordinate_x_relocate(int16_t x)
{
	return (x + LV_HOR_RES / 2);
}
static int16_t coordinate_y_relocate(int16_t y)
{
	return abs(y - LV_VER_RES / 2);
}

static lv_obj_t *screen_clock_create(clock_app_t *ht, lv_obj_t *parent)
{
       lv_obj_t *scr = lv_obj_create(parent, NULL);

	lv_style_init(&ht->style_line);
	lv_style_init(&ht->hour_line_style);
	lv_style_init(&ht->minute_line_style);
	lv_style_init(&ht->second_line_style);

	lv_style_set_line_width(&ht->style_line, LV_STATE_DEFAULT, 3);
	lv_style_set_line_color(&ht->style_line, LV_STATE_DEFAULT, LV_COLOR_PURPLE);
	lv_style_set_line_rounded(&ht->style_line, LV_STATE_DEFAULT, true);

	lv_style_set_line_width(&ht->hour_line_style, LV_STATE_DEFAULT, 7);
	lv_style_set_line_color(&ht->hour_line_style, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_style_set_line_rounded(&ht->hour_line_style, LV_STATE_DEFAULT, true);

	lv_style_set_line_width(&ht->minute_line_style, LV_STATE_DEFAULT, 5);
	lv_style_set_line_color(&ht->minute_line_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
	lv_style_set_line_rounded(&ht->minute_line_style, LV_STATE_DEFAULT, true);

	lv_style_set_line_width(&ht->second_line_style, LV_STATE_DEFAULT, 3);
	lv_style_set_line_color(&ht->second_line_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);

	ht->hour_body = lv_line_create(parent, NULL);
	ht->minute_body = lv_line_create(parent, NULL);
	ht->second_body = lv_line_create(parent, NULL);

	lv_obj_add_style(ht->hour_body, LV_LINE_PART_MAIN, &ht->hour_line_style);
	lv_obj_add_style(ht->minute_body, LV_LINE_PART_MAIN, &ht->minute_line_style);
	lv_obj_add_style(ht->second_body, LV_LINE_PART_MAIN, &ht->second_line_style);
	for(int i = 0; i < ARRAY_SIZE(gua_lines); i++) {
		lv_obj_t *line = lv_line_create(parent, NULL);

		lv_obj_add_style(line, LV_LINE_PART_MAIN, &ht->style_line);
		lv_line_set_points(line, (lv_point_t *)&gua_lines[i], 2);
	}
	return scr;
}

static void update_clock(clock_app_t *ht, struct tm *time)
{
	ht->hour_point[0].x = coordinate_x_relocate(0);
	ht->hour_point[0].y = coordinate_y_relocate(0);
	ht->hour_point[1].x = coordinate_x_relocate(HOUR_LENGTH * Sine((time->tm_hour > 12 ? time->tm_hour - 12 : time->tm_hour) * 30));
	ht->hour_point[1].y = coordinate_y_relocate(HOUR_LENGTH * Cosine((time->tm_hour > 12 ? time->tm_hour - 12 : time->tm_hour) * 30));

	ht->minute_point[0].x = coordinate_x_relocate(0);
	ht->minute_point[0].y = coordinate_y_relocate(0);
	ht->minute_point[1].x = coordinate_x_relocate(MINUTE_LENGTH * Sine(time->tm_min * 6) / LV_TRIG_SCALE);
	ht->minute_point[1].y = coordinate_y_relocate(MINUTE_LENGTH * Cosine(time->tm_min * 6) / LV_TRIG_SCALE);

	ht->second_point[0].x = coordinate_x_relocate(20 * Sine((180 + time->tm_sec * 6)) / LV_TRIG_SCALE);
	ht->second_point[0].y = coordinate_y_relocate(20 * Cosine((180 + time->tm_sec * 6)) / LV_TRIG_SCALE);
	ht->second_point[1].x = coordinate_x_relocate(SECOND_LENGTH * Sine(time->tm_sec * 6) / LV_TRIG_SCALE);
	ht->second_point[1].y = coordinate_y_relocate(SECOND_LENGTH * Cosine(time->tm_sec * 6) / LV_TRIG_SCALE);

	lv_line_set_points(ht->hour_body,   ht->hour_point,   2);
	lv_line_set_points(ht->minute_body, ht->minute_point, 2);
	lv_line_set_points(ht->second_body, ht->second_point, 2);
}

static void clock_update_task(lv_task_t *task)
{
	app_t *app = task->user_data;
	clock_app_t *htapp = _from_app(app);

	time_t now = (time_t)k_uptime_get() / 1000;
	struct tm *ptm = localtime(&now);

	update_clock(htapp, ptm);
}

static int clock_init(app_t *app, lv_obj_t *parent)
{
	clock_app_t *htapp = _from_app(app);
	htapp->screen = screen_clock_create(htapp, parent);

	htapp->lv_task_clock = lv_task_create(clock_update_task, 500, LV_TASK_PRIO_MID, app);

	return 0;
}

static int clock_event(app_t *app, uint32_t event, unsigned long data)
{
    return 0;
}

static int clock_exit(app_t *app)
{
	clock_app_t *ht = _from_app(app);

	lv_obj_clean(ht->screen);
	lv_obj_del(ht->screen);
	ht->screen = NULL;
	lv_task_del(ht->lv_task_clock);

	return 0;
}

static const app_spec_t clock_spec = {
	.name = "clock",
	.init = clock_init,
	.event = clock_event,
	.exit = clock_exit,
};