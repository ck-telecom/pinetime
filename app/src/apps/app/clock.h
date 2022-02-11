#ifndef _CLOCK_H
#define _CLOCK_H

#include <lvgl.h>

#include "../app.h"

typedef struct clock_app {
	app_t app;
	lv_obj_t *screen;
	lv_obj_t *lv_timeh;
	lv_obj_t *lv_timem;
	lv_obj_t *lv_times;
	lv_obj_t *lv_date;
	lv_obj_t *lv_ble;
	lv_obj_t *lv_power;
	lv_obj_t *lv_demo;
	//UTCTimeStruct time_old;

	lv_task_t *lv_task_clock;
} clock_app_t;

extern clock_app_t clock_app;
#define APP_CLOCK (&clock_app.app)

#endif /* _CLOCK_H */
