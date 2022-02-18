#ifndef _SERVICE_COMPASS_H
#define _SERVICE_COMPASS_H

typedef enum compass_status {
	COMPASS_STATUS_UNAVAILABLE,
	COMPASS_STATUS_DATA_INVALID,
	COMPASS_STATUS_CALIBRATING,
	COMPASS_STATUS_CALIBRATED,
} compass_status_t;

typedef int32_t compass_heading_t;

typedef struct compass_heading_data {
	compass_heading_t magnetic_heading;
	compass_status_t compass_status;
} compass_heading_data_t;

typedef void (*compass_heading_handler)(compass_heading_data_t heading, void *context);

struct compass_service_context {
	compass_heading_handler handler;
	void *context;
};

void compass_service_subscribe(compass_heading_handler handler, void *context);

void compass_service_unsubscribe(void);

#endif /* _SERVICE_COMPASS_H */