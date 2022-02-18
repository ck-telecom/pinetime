#ifndef _SERVICE_ACCEL_H
#define _SERVICE_ACCEL_H

struct accel_data {
	int16_t x;
	int16_t y;
	int16_t z;
	bool did_vibrate;
	uint64_t timestamp;
} accel_data_t;

typedef void (*accel_data_handler_t)(accel_data_t *data, uint32_t num_samples);

struct accel_data_service_context {
	uint32_t samples_per_update;
	accel_data_handler_t handler;
};

void accel_data_service_subscribe(uint32_t samples_per_update, accel_data_handler_t handler);

void accel_data_service_unsubscribe(void);

#endif /* _SERVICE_ACCEL_H */