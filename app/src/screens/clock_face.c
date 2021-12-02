#include <zephyr.h>
#include <lvgl.h>
#include <stdlib.h>
#include <time.h>
#include <logging/log.h>

#include "../view.h"
#include "../service/display_api.h"

LOG_MODULE_REGISTER(clock_face, LOG_LEVEL_INF);

static uint64_t uptime_ms;
static uint64_t last_uptime_ms;
static uint64_t elapsed_ms;
static struct tm ti;

static lv_style_t style_line;
static lv_style_t hour_line_style;
static lv_style_t minute_line_style;
static lv_style_t second_line_style;

static lv_obj_t *hour_body;
static lv_obj_t *minute_body;
static lv_obj_t *second_body;

static lv_point_t hour_point[2];
static lv_point_t minute_point[2];
static lv_point_t second_point[2];

static lv_task_t *lv_task_clock;

#ifndef PI
#define PI (3.141592653589793f)
#endif

#define HOUR_LENGTH     70
#define MINUTE_LENGTH   90
#define SECOND_LENGTH   110

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)  ( sizeof(a) / sizeof((a)[0]) )
#endif

#define LV_TRIG_SCALE   _lv_trigo_sin(90)

struct gua {
    lv_point_t line_point[2];
};


int16_t Cosine(int16_t angle) {
  return _lv_trigo_sin(angle + 90);
}

int16_t Sine(int16_t angle) {
  return _lv_trigo_sin(angle);
}

int16_t CoordinateXRelocate(int16_t x) {
  return (x + LV_HOR_RES / 2);
}

int16_t CoordinateYRelocate(int16_t y) {
  return abs(y - LV_VER_RES / 2);
}

static int16_t coordinate_x_relocate(int16_t x)
{
    return (x + LV_HOR_RES / 2);
}

static int16_t coordinate_y_relocate(int16_t y)
{
    return abs(y - LV_VER_RES / 2);
}

lv_point_t CoordinateRelocate(int16_t radius, int16_t angle) {
  lv_point_t point = {
    .x = CoordinateXRelocate(radius * (int32_t)(Sine(angle)) / LV_TRIG_SCALE),
    .y = CoordinateYRelocate(radius * (int32_t)(Cosine(angle)) / LV_TRIG_SCALE)
  };
  return point;
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

static void clock_update_elapsed_ms()
{
	uptime_ms = k_uptime_get();
	elapsed_ms = uptime_ms - last_uptime_ms;
	last_uptime_ms = uptime_ms;
}

void clock_increment_local_time()
{
	clock_update_elapsed_ms();

	ti.tm_sec += elapsed_ms / 1000;
	// mktime(&ti);
}

static void update_clock(struct tm *time);

static void home_task(lv_task_t *task)
{
    time_t now = (time_t)k_uptime_get() / 1000;
    struct tm *ptm = localtime(&now);
    update_clock(ptm);
}

static void update_clock(struct tm *time)
{
    hour_point[0].x = coordinate_x_relocate(0);
    hour_point[0].y = coordinate_y_relocate(0);
    hour_point[1].x = coordinate_x_relocate(HOUR_LENGTH * Sine((time->tm_hour > 12 ? time->tm_hour - 12 : time->tm_hour) * 30));
    hour_point[1].y = coordinate_y_relocate(HOUR_LENGTH * Cosine((time->tm_hour > 12 ? time->tm_hour - 12 : time->tm_hour) * 30));

    minute_point[0].x = coordinate_x_relocate(0);
    minute_point[0].y = coordinate_y_relocate(0);
    minute_point[1].x = coordinate_x_relocate(MINUTE_LENGTH * Sine(time->tm_min * 6) / LV_TRIG_SCALE);
    minute_point[1].y = coordinate_y_relocate(MINUTE_LENGTH * Cosine(time->tm_min * 6) / LV_TRIG_SCALE);

    second_point[0].x = coordinate_x_relocate(20 * Sine((180 + time->tm_sec * 6)) / LV_TRIG_SCALE);
    second_point[0].y = coordinate_y_relocate(20 * Cosine((180 + time->tm_sec * 6)) / LV_TRIG_SCALE);
    second_point[1].x = coordinate_x_relocate(SECOND_LENGTH * Sine(time->tm_sec * 6) / LV_TRIG_SCALE);
    second_point[1].y = coordinate_y_relocate(SECOND_LENGTH * Cosine(time->tm_sec * 6) / LV_TRIG_SCALE);

    lv_line_set_points(hour_body,   hour_point,   2);
    lv_line_set_points(minute_body, minute_point, 2);
    lv_line_set_points(second_body, second_point, 2);
}

int clock_face_init(struct view *v, lv_obj_t *parent)
{
    lv_style_init(&style_line);
    lv_style_init(&hour_line_style);
    lv_style_init(&minute_line_style);
    lv_style_init(&second_line_style);

    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 3);
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_PURPLE);
    lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, true);

    lv_style_set_line_width(&hour_line_style, LV_STATE_DEFAULT, 7);
    lv_style_set_line_color(&hour_line_style, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_line_rounded(&hour_line_style, LV_STATE_DEFAULT, true);

    lv_style_set_line_width(&minute_line_style, LV_STATE_DEFAULT, 5);
    lv_style_set_line_color(&minute_line_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_line_rounded(&minute_line_style, LV_STATE_DEFAULT, true);

    lv_style_set_line_width(&second_line_style, LV_STATE_DEFAULT, 3);
    lv_style_set_line_color(&second_line_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);

    hour_body = lv_line_create(parent, NULL);
    minute_body = lv_line_create(parent, NULL);
    second_body = lv_line_create(parent, NULL);

    lv_obj_add_style(hour_body, LV_LINE_PART_MAIN, &hour_line_style);
    lv_obj_add_style(minute_body, LV_LINE_PART_MAIN, &minute_line_style);
    lv_obj_add_style(second_body, LV_LINE_PART_MAIN, &second_line_style);

    for(int i = 0; i < ARRAY_SIZE(gua_lines); i++) {
        lv_obj_t *line = lv_line_create(parent, NULL);
        lv_obj_add_style(line, LV_LINE_PART_MAIN, &style_line);

        lv_line_set_points(line, (lv_point_t *)&gua_lines[i], 2);
    }

    lv_task_clock = lv_task_create(home_task, 500, LV_TASK_PRIO_MID, NULL);

    return 0;
}

int clock_face_exit(struct view *v, lv_obj_t *parent)
{
    lv_task_del(lv_task_clock);

    lv_obj_clean(parent);
}

bool clock_face_event_handler(struct view *view, uint32_t event, void *arg)
{
    uint32_t gesture = *(uint32_t *)arg;
    if (gesture == 0x02) {
        view_switch_screen(view, &date_settings);
    }
}

struct view clock_face_view = {
    .id = CLOCK_FACE_ID,
    .name = "clock_face",
    .init = clock_face_init,
    .event = clock_face_event_handler,
    .exit = clock_face_exit,
};