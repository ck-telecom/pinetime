#include "app_worker.h"

bool app_worker_is_running(void)
{
}

bool app_worker_message_subscribe(AppWorkerMessageHandler handler)
{
	bool ret = false;

	k_tid_t this_thread = k_current_get();
	if (this_thread == app) {
		ret = app_event_service_subscribe(handler);
	} else {
		ret = app_worker_event_service_subscrible(handler);
	}

	return ret;
}

bool app_worker_message_unsubscribe(void)
{
	bool ret = false;

	k_tid_t this_thread = k_current_get();
	if (this_thread == app) {
		ret = app_event_service_unsubscribe(handler);
	} else {
		ret = app_worker_event_service_unsubscrible(handler);
	}

	return ret;
}

void app_worker_send_message(uint8_t type, AppWorkerMessage *data)
{
	if (type == SOURCE_BACKGROUND) {
		app_worker_event_post();
	} else {
		app_event_post();
	}
}
