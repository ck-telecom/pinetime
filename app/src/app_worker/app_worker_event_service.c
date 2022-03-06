#include <zephyr.h>

#include "app_worker_event_service.h"

K_MSGQ_DEFINE(app_work_msgq, sizeof(struct app_worker_message), 8, 4);

int app_worker_event_service_post(void *data)
{
	return k_msgq_put(&app_worker_msgq, data, K_NO_WAIT);
}

void app_worker_event_trigger(struct app_worker_message *message)
{
	SYS_SLIST_FOR_EACH_CONTAINER(app_worker_eh, subscri, node) {
		subscri->handler(message->type, message->data);
	}
}

void app_work_thread(void)
{
	struct app_worker_message message;

	while (1) {
		k_msgq_get(&app_work_msgq, &message, K_FOREVER);

		app_worker_event_trigger(&message);
	}
}
K_THREAD_DEFINE(app_worker_tid, VIBES_STACK_SIZE, app_work_thread,
				NULL, NULL, NULL,
				VIBRAES_PRIORITY, 0, 0);