#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>

#include <bluetooth/gatt.h>

/* Alert Notification Service Client */
#include "ans_client.h"

#define LOG_MODULE_NAME bt_ans_client
#include "common/log.h"

static uint8_t ans_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				 struct bt_gatt_discover_params *params)
{
	struct bt_ans_client *client_inst = CONTAINER_OF(params,
							  struct bt_ans_client,
							  discover_params);

	struct bt_ans *inst = CONTAINER_OF(client_inst, struct bt_ans, cli);

	if (!attr) {
		BT_DBG("ANS Discovery completed");
		return BT_GATT_ITER_STOP;
	}

	if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		struct bt_gatt_chrc *chrc =(struct bt_gatt_chrc *)attr->user_data;
		struct bt_gatt_subscribe_params *sub_params = NULL;

		BT_DBG("Discovered attribute - uuid: %s, handle: %u", bt_uuid_str(chrc->uuid), attr->handle);
		if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANS_ALERT_NOTIFICATION_CONTROL_POINT)) {
			BT_DBG("ANS Alert Notification Control Point");

		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANS_UNREAD_ALERT_STATUS)) {
			BT_DBG("ANS Unread Alert Status");

		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANS_NEW_ALERT)) {
			BT_DBG("ANS New Alert");

			//inst->cli.data_soure_handle = bt_gatt_attr_value_handle(attr);
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANS_SUPPORTED_NEW_ALERT_CATEGORY)) {
			BT_DBG("ANS Supported New Alert Category");

		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANS_SUPPORTED_UNREAD_ALERT_CATEGORY)) {
			BT_DBG("ANS Supported Unread Alert Category");

		}

		if (sub_params) {
			bt_gatt_subscribe(conn, sub_params);
		}
	}

	return BT_GATT_ITER_CONTINUE;
}

static uint8_t primary_discover_func(struct bt_conn *conn,
				     const struct bt_gatt_attr *attr,
				     struct bt_gatt_discover_params *params)
{
	struct bt_ans_client *client_inst = CONTAINER_OF(params,
							 struct bt_ans_client,
							 discover_params);

	struct bt_ans *inst = CONTAINER_OF(client_inst, struct bt_ans, cli);

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
		inst->cli.discover_params.func = ans_discover_func;

		err = bt_gatt_discover(conn, &inst->cli.discover_params);
		if (err) {
			BT_DBG("Discover failed (err %d)", err);
		}
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

int bt_ans_discover(struct bt_conn *conn, struct bt_ans *inst)
{
	int err = 0;

	memcpy(&inst->cli.uuid, BT_UUID_ANS, sizeof(inst->cli.uuid));

	inst->cli.conn = conn;
	inst->cli.discover_params.func = primary_discover_func;
	inst->cli.discover_params.uuid = &inst->cli.uuid.uuid;
	inst->cli.discover_params.type = BT_GATT_DISCOVER_PRIMARY;
	inst->cli.discover_params.start_handle = BT_ATT_FIRST_ATTTRIBUTE_HANDLE;
	inst->cli.discover_params.end_handle = BT_ATT_LAST_ATTTRIBUTE_HANDLE;

	err = bt_gatt_discover(conn, &inst->cli.discover_params);

	return err;
}