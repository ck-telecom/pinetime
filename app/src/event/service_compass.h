
typedef struct compass_heading_data {

} compass_heading_data_t;

typedef void (*compass_heading_handler)(compass_heading_data_t heading, void *context);

struct compass_service_context {
	compass_heading_handler handler;
	void *context;
};