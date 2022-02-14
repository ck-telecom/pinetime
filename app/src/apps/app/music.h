#ifndef _APP_MUSIC_H
#define _APP_MUSIC_H

#include <lvgl.h>

#include "../app.h"

typedef struct app_music {
	app_t app;
	lv_obj_t *screen;

	lv_task_t *task;
} app_music_t;

extern app_music_t app_music;
#define APP_MUSIC (&app_music.app)

#endif /* _APP_MUSIC_H */
