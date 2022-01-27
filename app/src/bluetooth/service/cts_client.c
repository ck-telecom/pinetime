
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

#include "cts_internal.h"
#include "cts.h"

#define LOG_MODULE_NAME bt_cts_client
#include "common/log.h"

static uint8_t cts_client_read_cb(struct bt_conn *conn, uint8_t err,
			   struct bt_gatt_read_params *params,
			   const void *data, uint16_t length)
{
	struct bt_cts_client *client = CONTAINER_OF(params,
						    struct bt_cts_client,
						    discover_params);

	struct bt_cts *inst = CONTAINER_OF(client,
					   struct bt_cts,
					   cli);
	uint8_t *p = data;
	BT_DBG("Reading CCC data: err %d, %d bytes", err, length);
	/*for (uint16_t i = 0; i < length; ++i) {
		printk("0x%x", p[i]);
	}*/
	if (err) {

	} else if (data) {
		if (length ==  10) {
			memcpy(inst->cli.cts_buf, data, length);
			struct cts_current_time *ctime = inst->cli.cts_buf;
			BT_DBG("Year%04d Day%03d T%2d:%2d:%2d",
				ctime->year, ctime->day,
				cts_time->hours, ctime->minutes, ctime->seconds);
		}
	} else {

	}
	/* callback to notify? */
	return BT_GATT_ITER_CONTINUE;
}

static uint8_t cts_current_time_notify(struct bt_conn* conn,
				       struct bt_gatt_subscribe_params* params,
				       const void* data, uint16_t length)
{

}

int bt_cts_client_time_get(struct bt_cts *inst)
{
	int err = 0;

	inst->cli.read_params.func = cts_client_read_cb;
	inst->cli.read_params.handle_count = 1;
	inst->cli.read_params.single.handle = inst->cli.current_time_handle;
	inst->cli.read_params.single.offset = 0;

	err = bt_gatt_read(inst->cli.conn, &inst->cli.read_params);
	if (err < 0) {
		BT_DBG("Could get time");
	}

	return err;
}

static uint8_t cts_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				  struct bt_gatt_discover_params *params)
{
	struct bt_cts_client *client = CONTAINER_OF(params,
						    struct bt_cts_client,
						    discover_params);

	struct bt_cts *inst = CONTAINER_OF(client,
					   struct bt_cts,
					   cli);

	if (!attr) {
		BT_DBG("CTS Discovery completed");
		bt_cts_client_time_get(inst);
		return BT_GATT_ITER_STOP;
	}

	if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		struct bt_gatt_chrc *chrc =(struct bt_gatt_chrc *)attr->user_data;
		struct bt_gatt_subscribe_params *sub_params = NULL;
		// BT_DBG("Discovered attribute - uuid: %s, handle: %u", bt_uuid_str(chrc->uuid), attr->handle);

		if (!bt_uuid_cmp(chrc->uuid, BT_UUID_CTS_CURRENT_TIME)) {
			BT_DBG("CTS Current Time");
			inst->cli.current_time_handle = bt_gatt_attr_value_handle(attr);

			sub_params = &inst->cli.subscribe_parms;

			sub_params->value = BT_GATT_CCC_NOTIFY;
			sub_params->notify = cts_current_time_notify;
			sub_params->value_handle = bt_gatt_attr_value_handle(attr);
#if defined(CONFIG_BT_GATT_AUTO_DISCOVER_CCC)
			sub_params->ccc_handle = 0;
			sub_params->end_handle = inst->cli.end_handle;
			sub_params->disc_params = &inst->cli.discover_params;
#else
			#error "CONFIG_BT_GATT_AUTO_DISCOVER_CCC not configured"
#endif
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_CTS_LOCAL_TIME_INFOMATION)) {
			BT_DBG("CTS Local Time Information");
		}

		if (sub_params) {
			bt_gatt_subscribe(inst->cli.conn, sub_params);
		}
	}

	return BT_GATT_ITER_CONTINUE;
}

static uint8_t primary_discover_func(struct bt_conn *conn,
				     const struct bt_gatt_attr *attr,
				     struct bt_gatt_discover_params *params)
{
	struct bt_cts_client *client_inst = CONTAINER_OF(params,
							 struct bt_cts_client,
							 discover_params);

	struct bt_cts *inst = CONTAINER_OF(client_inst, struct bt_cts, cli);

	if (params->type == BT_GATT_DISCOVER_PRIMARY) {
		int err = 0;
		struct bt_gatt_service_val* gatt_service = attr->user_data;
		BT_DBG("Primary discover complete");

		inst->cli.start_handle = attr->handle + 1;
		inst->cli.end_handle = gatt_service->end_handle;

		inst->cli.discover_params.uuid = NULL;
		inst->cli.discover_params.start_handle = inst->cli.start_handle;
		inst->cli.discover_params.end_handle = inst->cli.end_handle;
		inst->cli.discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
		inst->cli.discover_params.func = cts_discover_func;

		err = bt_gatt_discover(conn, &inst->cli.discover_params);
		if (err) {
			BT_DBG("Discover failed (err %d)", err);
		}
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

int bt_cts_discover(struct bt_conn *conn, struct bt_cts *inst)
{
	int err = 0;

	memcpy(&inst->cli.uuid, BT_UUID_CTS, sizeof(inst->cli.uuid));

	inst->cli.conn = conn;
	inst->cli.discover_params.func = primary_discover_func;
	inst->cli.discover_params.uuid = &inst->cli.uuid.uuid;
	inst->cli.discover_params.type = BT_GATT_DISCOVER_PRIMARY;
	inst->cli.discover_params.start_handle = BT_ATT_FIRST_ATTTRIBUTE_HANDLE;
	inst->cli.discover_params.end_handle = BT_ATT_LAST_ATTTRIBUTE_HANDLE;

	err = bt_gatt_discover(conn, &inst->cli.discover_params);

	return err;
}
