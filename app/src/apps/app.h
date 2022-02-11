#ifndef _APP_H
#define _APP_H

#include <stdbool.h>

#include <lvgl.h>

#include "msg_def.h"

typedef struct _app app_t;

typedef struct app_spec {
	const char *name;
	int (*init)(app_t *app, lv_obj_t *parent);
	int (*event)(app_t *app, uint32_t event, unsigned long data);
	int (*exit)(app_t *app);
} app_spec_t;

struct _app {
	const app_spec_t *spec;
	bool dirty;
};

enum apps {
	HOME,
	CLOCK,
	DATE_SETTINGS_ID
};

enum app_state {
	APP_RUNNING,
	APP_SLEEP
};

struct app_context {
//     enum appGestures gestureDir;
//     enum RefreshDirections refreshDirection;
	enum app_state state;
	app_t *running_app;
	enum apps active_app;
	enum apps return_app;
     //enum appGestures returnDir;
     //enum RefreshDirections returnAnimation;
};

void load_application(enum apps app/*, enum RefreshDirections dir*/);

void app_push_msg(enum msg_type msg_type);

#endif /* _APP_H */