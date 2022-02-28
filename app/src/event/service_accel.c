#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>

#include "event_service.h"
#include "service_accel.h"

#include "msg_def.h"

#define ACCEL_DEV bosch_bma4xx

const struct device *dev;

static void accel_data_service_cb(uint32_t command, void *data, void *context)
{
	struct accel_data_service_context *srv_ctx = context;
	struct msg_accel_data *msg = (struct msg_accel_data *)data;

	if (srv_ctx->handler) {
		srv_ctx->handler(msg->sensor_data, msg->num_samples, srv_ctx->context);
	}
}

int accel_service_peek(struct accel_data *data)
{
	int err = 0;
	struct sensor_value accel_data[3] = { 0 };

	if (!dev) {
		return -ENODEV;
	}

	err = sensor_sample_fetch(dev);
	if (err < 0) {
		LOG_ERR("sensor_sample_fetch error");
	}

	err = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel_data);
	if (err < 0) {
		LOG_ERR("sensor_channel_get error");
	}

	data->x = sensor_value_to_double(&accel_data[0]);
	data->y = sensor_value_to_double(&accel_data[1]);
	data->z = sensor_value_to_double(&accel_data[2]);

	return err;
}

int accel_service_set_sampling_rate(AccelSamplingRate rate)
{
	struct sensor_value setting;

	(void)sensor_value_from_double(&setting, (double)rate);

	return sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &setting);
}

void accel_data_service_subscribe(uint32_t samples_per_update, accel_data_handler_t handler)
{
	accel_data_handler_t *handler = app_alloc(sizeof(accel_data_handler_t));

	//TODO: set sensor attr samples_per_update;

	event_service_subscribe_with_context(MSG_TYPE_ACCEL_RAW, accel_data_service_cb, handler);
}

void accel_data_service_unsubscribe(void)
{
	void *context = event_service_get_context(MSG_TYPE_ACCEL_RAW);
	if (context)
		app_free(context);

	event_service_unsubscribe(MSG_TYPE_ACCEL_RAW);
}

static int accel_service_init(const struct device *dev)
{
	dev = DEVICE_DT_GET_ONE(ACCEL_DEV);

	if (!device_is_ready(dev)) {
		printf("Sensor %s is not ready\n", dev->name);
		return -ENODEV;
	}

	return 0;
}
SYS_INIT(accel_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
