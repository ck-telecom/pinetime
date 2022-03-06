#ifndef _APP_WORKER_H
#define _APP_WORKER_H

#include <stdbool.h>

enum {
	SOURCE_BACKGROUND,
	SOURCE_FOREGROUND
};

typedef struct AppWorkerMessage {
	uint16_t data0;
	uint16_t data1;
	uint16_t data2;
} AppWorkerMessage;

typedef void (*AppWorkerMessageHandler)(uint16_t type, AppWorkerMessage *data);

bool app_worker_is_running(void);

bool app_worker_message_subscribe(AppWorkerMessageHandler handler);

bool app_worker_message_unsubscribe(void);

void app_worker_send_message(uint8_t type, AppWorkerMessage *data);

#endif /* _APP_WORKER_H */