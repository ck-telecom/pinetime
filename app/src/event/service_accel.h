#ifndef _SERVICE_ACCEL_H
#define _SERVICE_ACCEL_H

struct accel_data {
	int16_t x;
	int16_t y;
	int16_t z;
	bool did_vibrate;
	uint64_t timestamp;
} accel_data_t;

typedef enum accel_sampling_rate {
	ACCEL_SAMPLING_10HZ,
	ACCEL_SAMPLING_25HZ,
	ACCEL_SAMPLING_50HZ,
	ACCEL_SAMPLING_100HZ,
} AccelSamplingRate;

typedef void (*accel_data_handler_t)(accel_data_t *data, uint32_t num_samples);

struct accel_data_service_context {
	uint32_t samples_per_update;
	accel_data_handler_t handler;
};

int accel_service_peek(struct accel_data *data);

int accel_service_set_sampling_rate(AccelSamplingRate rate);

void accel_data_service_subscribe(uint32_t samples_per_update, accel_data_handler_t handler);

void accel_data_service_unsubscribe(void);

#endif /* _SERVICE_ACCEL_H */