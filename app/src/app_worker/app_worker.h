#ifndef _APP_WORKER_H
#define _APP_WORKER_H

#include <stdbool.h>

struct app_worker_message {
	uint16_t data0;
	uint16_t data1;
	uint16_t data2;
} app_worker_message_t;

typedef void (*app_worker_message_handler_t)(uint16_t type, app_worker_message_t *data);

bool app_worker_is_running(void);

bool app_worker_message_subscribe(app_worker_message_handler_t handler);

bool app_worker_message_unsubscribe(void);

void app_worker_send_message(uint8_t type, app_worker_message_t *data);

#endif /* _APP_WORKER_H */