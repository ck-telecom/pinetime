#include "event_service.h"
#include "service_accel.h"

#include "msg_def.h"

static void accel_data_service_cb(uint32_t command, void *data, void *context)
{
	struct accel_data_service_context *srv_ctx = context;
	struct msg_accel_data *msg = (struct msg_accel_data *)data;

	if (srv_ctx->handler) {
		srv_ctx->handler(msg->sensor_data, msg->num_samples, srv_ctx->context);
	}
}

void accel_data_service_subscribe(uint32_t samples_per_update, AccelDataHandler handler)
{
	AccelDataHandler *handler = app_alloc(sizeof(AccelDataHandler));

	//TODO: set sensor attr samples_per_update;

	event_service_subscribe_with_context(MSG_TYPE_ACCEL_RAW, accel_data_service_cb, handler);
}
