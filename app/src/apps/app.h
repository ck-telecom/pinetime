#ifndef _APP_H
#define _APP_H

#include <stdbool.h>

typedef struct _app app_t;

typedef struct app_spec {
	const char *name;
//	uint32_t updateInterval;
	int (*init)(app_t *app, lv_obj_t *parent);
//	int (*update)(app_t *app);
	int (*event_handler)(app_t *app, uint32_t event, unsigned long data);
	int (*eixt)(app_t *app);
} app_spec_t;

struct _app {
	const app_spec_t *spec;
	bool dirty;
};

enum appMessages {
     UpdateBleConnection    = 0x01,
     UpdateBatteryLevel     = 0x02,
     Timeout                = 0x03,
     ButtonPushed           = 0x04,
     WakeUp                 = 0x05,
     Charging               = 0x06,
     Gesture                = 0x07,
     NewNotification        = 0x08,
     ShowPasskey            = 0x09,
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

void app_push_message(enum appMessages msg);
void load_application(enum apps app/*, enum RefreshDirections dir*/);

#endif /* _APP_H */