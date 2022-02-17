#include "service_compass.h"

static void compass_service_cb(uint32_t command, void *data, void *context)
{
	struct compass_service_context *srv_ctx = context;
	compass_heading_data_t *heading_data = data;

	if (srv_ctx->handler) {
		srv_ctx->handler(heading_data, srv_ctx->context);
	}
}

compass_service_subscribe(compass_heading_handler handler, void *context)
{
	struct compass_service_context *srv_ctx = app_alloc(struct compass_service_context);

	srv_ctx->hander = handler;
	srv_ctx->context = context;

	event_service_subscribe_with_context(MSG_TYPE_COMPASS, compass_service_cb, srv_ctx);
}
