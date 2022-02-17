#ifndef _SERVICE_ACCEL_H
#define _SERVICE_ACCEL_H

struct AccelData {
	int16_t x;
	int16_t y;
	int16_t z;
	bool did_vibrate;
	uint64_t timestamp;
};

typedef void(* AccelDataHandler)(AccelData *data, uint32_t num_samples);

struct accel_data_service_context {
	uint32_t samples_per_update;
	AccelDataHandler handler;
};

#endif /* _SERVICE_ACCEL_H */