#include <bluetooth/gatt.h>

#include "ancs_c.h"

#define LOG_MODULE_NAME bt_ancs_client
#include "common/log.h"

static uint8_t ancs_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				 struct bt_gatt_discover_params *params)
{
	struct bt_ancs_client *client_inst = CONTAINER_OF(params,
							  struct bt_ancs_client,
							  discover_params);

	struct bt_ancs *inst = CONTAINER_OF(client_inst, struct bt_ancs, cli);

	if (!attr) {
		BT_DBG("ANCS Discovery completed");
		return BT_GATT_ITER_STOP;
	}

	if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		struct bt_gatt_chrc *chrc =(struct bt_gatt_chrc *)attr->user_data;

		//BT_DBG("Discovered attribute - uuid: %s, handle: %u\n", bt_uuid_str(chrc->uuid), attr->handle);
		if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANCS_NOTIFICATION_SOURCE)) {
			BT_DBG("ANCS Notification Source");
			inst->cli.notification_soure_handle = attr->handle + 1;
			//inst->cli.entity_subscribe_handle = attr->handle + 1;
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANCS_CTRL_POINT)) {
			inst->cli.control_point_handle = attr->handle + 1;
			BT_DBG("ANCS Control Point");
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANCS_DATA_SOURCE)) {
			inst->cli.data_soure_handle = attr->handle + 1;
			BT_DBG("ANCS Data Source");
		}
	}

	return BT_GATT_ITER_CONTINUE;
}

static uint8_t primary_discover_func(struct bt_conn *conn,
				     const struct bt_gatt_attr *attr,
				     struct bt_gatt_discover_params *params)
{
	struct bt_ancs_client *client_inst = CONTAINER_OF(params,
							 struct bt_ancs_client,
							 discover_params);

	struct bt_ancs *inst = CONTAINER_OF(client_inst, struct bt_ancs, cli);

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
		inst->cli.discover_params.func = ancs_discover_func;

		err = bt_gatt_discover(conn, &inst->cli.discover_params);
		if (err) {
			BT_DBG("Discover failed (err %d)", err);
		}
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

int bt_ancs_discover(struct bt_conn *conn, struct bt_ancs *inst)
{
	int err = 0;

	memcpy(&inst->cli.uuid, BT_UUID_ANCS, sizeof(inst->cli.uuid));

	inst->cli.conn = conn;
	inst->cli.discover_params.func = primary_discover_func;
	inst->cli.discover_params.uuid = &inst->cli.uuid.uuid;
	inst->cli.discover_params.type = BT_GATT_DISCOVER_PRIMARY;
	inst->cli.discover_params.start_handle = BT_ATT_FIRST_ATTTRIBUTE_HANDLE;
	inst->cli.discover_params.end_handle = BT_ATT_LAST_ATTTRIBUTE_HANDLE;

	err = bt_gatt_discover(conn, &inst->cli.discover_params);

	return err;
}
