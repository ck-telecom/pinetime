
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

#include "cts_internal.h"

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

	BT_DBG("Reading CCC data: err %d, %d bytes", err, length);

	if (err) {

	} else if (data) {
		if (length ==  10) {
			memcpy(inst->cli.cts_buf, data, length);
		}
	} else {

	}
	/* callback to notify? */
	return BT_GATT_ITER_CONTINUE;
}

int bt_cts_client_time_get(struct bt_cts *inst)
{
	int err = 0;

	inst->cli.read_params.func = cts_client_read_cb;
	inst->cli.read_params.handle_count = 1;
	inst->cli.read_params.single.handle = inst->cli.time_handle;
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
		BT_DBG("CTS Service Discovery completed");
		return BT_GATT_ITER_STOP;
	}
	BT_DBG("Discovered attribute, handle: %u\n", attr->handle);

	if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		if (!bt_uuid_cmp(params->uuid, BT_UUID_CTS_CURRENT_TIME)) {
			inst->cli.time_handle = attr->handle;
			bt_cts_client_time_get(inst);
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
