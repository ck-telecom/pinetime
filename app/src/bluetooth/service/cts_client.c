
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
	static uint8_t offset = 0;

	BT_DBG("Reading Current Time: err %d, %d bytes", err, length);

	if (err) {

	} else if (data && (length > 0)) {
		memcpy(inst->cli.cts_buf + offset, data, length);
		offset += length;
	} else {

	}
	if (offset >= 10) {
		struct cts_current_time *ctime = (struct cts_current_time *)inst->cli.cts_buf;
		BT_DBG("Year %04d Day %02d Time %2d:%2d:%2d",
			ctime->year, ctime->day,
			ctime->hours, ctime->minutes, ctime->seconds);
	}
	/* callback to notify? */
	return BT_GATT_ITER_CONTINUE;
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

static bool valid_inst_discovered(struct bt_cts *inst)
{
	return inst->cli.current_time_handle;
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
		inst->cli.busy = false;
		(void)memset(params, 0, sizeof(*params));

		if (inst->cli.cb && inst->cli.cb->discover) {
			int err = valid_inst_discovered(inst) ? 0 : -ENOENT;

			inst->cli.cb->discover(inst, err);
		}

		return BT_GATT_ITER_STOP;
	}

	if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		struct bt_gatt_chrc *chrc =(struct bt_gatt_chrc *)attr->user_data;

		// BT_DBG("Discovered attribute - uuid: %s, handle: %u", bt_uuid_str(chrc->uuid), attr->handle);

		if (!bt_uuid_cmp(chrc->uuid, BT_UUID_CTS_CURRENT_TIME)) {
			BT_DBG("CTS Current Time");
			inst->cli.current_time_handle = bt_gatt_attr_value_handle(attr);
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_CTS_LOCAL_TIME_INFOMATION)) {
			BT_DBG("CTS Local Time Information");
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

	if (attr == NULL) {
		BT_DBG("Could not find a CTS instance on the server");
		if (inst->cli.cb && inst->cli.cb->discover) {
			inst->cli.cb->discover(inst, -ENODATA);
		}
		return BT_GATT_ITER_STOP;
	}

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
			if (inst->cli.cb && inst->cli.cb->discover) {
				inst->cli.cb->discover(inst, err);
			}
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
	if (err) {
		BT_DBG("Discover failed (err %d)", err);
	} else {
		inst->cli.busy = true;
	}

	return err;
}

void bt_cts_client_cb_register(struct bt_cts *inst, struct bt_cts_cb *cb)
{
	if (!inst) {
		BT_DBG("inst cannot be NULL");
		return;
	}

	inst->cli.cb = cb;
}
