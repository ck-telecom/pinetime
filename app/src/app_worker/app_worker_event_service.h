#include "app_worker.h"

struct app_worker_message {
	AppWorkerMessage data;
	uint16_t type;
};