#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>
#include <logging/log.h>

#include <lvgl.h>

#include "app.h"
#include "app/clock.h"

#define APP_STACK_SIZE      1024
#define APP_PRIORITY        5

static struct app_context context;
static struct app_context pinetimecosapp;
K_MBOX_DEFINE(app_mailbox);

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

static uint8_t app_buffer[1024];

int app_init(app_t *app, lv_obj_t *parent)
{
	if (app->spec->init)
		return app->spec->init(app, parent);

	return 0;
}

int app_exit(app_t *app)
{
	if (app->spec->exit)
		return app->spec->exit(app);

	return 0;
}

int app_event(app_t *app, uint32_t event, unsigned long data)
{
	if (app->spec->event)
		return app->spec->event(app, event, data);

	return 0;
}

int app_generic_event_handler(app_t *app, uint32_t event, unsigned long data)
{
	return event_service_event_trigger(app, event, data);
}

static void run_app(app_t *app)
{
	if (app == pinetimecosapp.running_app) {
		return;
	}
	//pinetimecos.lvglstate = Sleep;
	//lv_timer_pause(app_timer);
	if (pinetimecosapp.running_app) {
		app_exit(pinetimecosapp.running_app);
	}
	pinetimecosapp.running_app = app;

	app_init(pinetimecosapp.running_app, lv_scr_act());
	//lv_timer_set_period(app_timer, pinetimecosapp.running_app->spec->updateInterval);

	//pinetimecos.lvglstate = Running;
	//lv_timer_resume(app_timer);
}

static void return_app(enum apps app/*, enum appGestures gesture, enum RefreshDirections dir*/)
{
	pinetimecosapp.return_app = app;
	//pinetimecosapp.returnAnimation = dir;
	//pinetimecosapp.returnDir = gesture;
}

void load_application(enum apps app/*, enum RefreshDirections dir*/)
{
    //set_refresh_direction(dir);
    //pinetimecosapp.activeApp = app;
	switch (app) {
/*
	case Passkey:
		run_app(APP_PASSKEY);
		return_app(Clock, DirBottom, AnimNone);
		break;

	case Debug:
		run_app(APP_DEBUG);
		return_app(Menu, DirTop, AnimUp);
		break;



	case Menu:
		run_app(APP_MENU);
		return_app(Clock, DirTop, AnimUp);
		break;

	case Notification:
		run_app(APP_NOTIFICATION);
		return_app(Clock, DirBottom, AnimDown);
		break;
*/
	case CLOCK:
		run_app(APP_CLOCK);
		return_app(CLOCK/*, DirNone, AnimNone*/);
		break;

	default:
		break;
	}
}

void app_push_msg(enum msg_type msg_type)
{
	struct k_mbox_msg send_msg;

	send_msg.info = msg_type;
	send_msg.size = 0;
	send_msg.tx_data = NULL;
	send_msg.tx_block.data = NULL;
	send_msg.tx_target_thread = K_ANY;

	k_mbox_async_put(&app_mailbox, &send_msg, NULL);
}

void app_push_msg_with_data(enum msg_type msg_type, void *data, size_t size)
{
	struct k_mbox_msg send_msg;

	send_msg.info = msg_type;
	send_msg.size = size;
	send_msg.tx_data = data;
	send_msg.tx_block.data = NULL;
	send_msg.tx_target_thread = K_ANY;

	k_mbox_async_put(&app_mailbox, &send_msg, NULL);
}

void app_thread(void* arg1, void *arg2, void *arg3)
{
	//struct app_context *context = (struct app_context*)arg1;
	const struct device *display_dev = NULL;
	struct k_mbox_msg recv_msg;
	int ret;
	k_timeout_t timeout = K_MSEC(10);

	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device %s not found", CONFIG_LVGL_DISPLAY_DEV_NAME);
		return;
	}

	display_blanking_off(display_dev);

	//context->state = APP_RUNNING;
	load_application(CLOCK);
	//context->active_app = APP_CLOCK;

	while (1) {
		recv_msg.info = -1;
		recv_msg.info = 1024;
		recv_msg.rx_source_thread = K_ANY;

		ret = k_mbox_get(&app_mailbox, &recv_msg, app_buffer, timeout);

		switch (recv_msg.info) {
		case MSG_TYPE_BLE_CONNECTED:
			/* code */
			break;

		case MSG_TYPE_BLE_DISCONNECTED:
			break;

		default:
			break;
		}

		if (pinetimecosapp.running_app) {
			app_event(pinetimecosapp.running_app, recv_msg.info, app_buffer);
		}

#if (LVGL_VERSION_MAJOR < 8)
		lv_task_handler();
#else
		lv_timer_handler();
#endif
	}
}
K_THREAD_DEFINE(app, APP_STACK_SIZE, app_thread, &context, NULL, NULL, APP_PRIORITY, 0, 0);
