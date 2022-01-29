#include <bluetooth/gatt.h>

#include "ancs_c.h"

#define LOG_MODULE_NAME bt_ancs_client
#include "common/log.h"

static const char *event_id_str[ANCS_EVENT_ID_MAX] = {
	"Add",
	"Modified",
	"Removed"
};

static const char *category_id_str[ANCS_CATEGORY_ID_MAX] = {
	"Other",
	"Incoming Call",
	"Missed Call",
	"Voice Mail",
	"Social",
	"Schedule",
	"E-mail",
	"News",
	"Health and Fitness",
	"Buisness and Finance",
	"Location",
	"Entertainment"
};

int bt_ancs_client_get_notif_attr(struct bt_ancs *inst,
				  enum ancs_command_id cmd_id,
				  enum ancs_notif_attr_id attr_id,
				  ...)
{

}

int bt_ancs_client_get_app_attrs(struct bt_ancs *inst, ...)
{

}

int bt_ancs_client_perform_notif_action(struct bt_ancs *inst,
					uint32_t notif_uid,
					enum ble_ancs_c_action_id action_id)
{
	memset(&inst->cli.write_params, 0, sizeof(inst->cli.write_params));
	inst->cli.buff[0] = ANCS_COMMAND_ID_GET_PERFORM_NOTIF_ACTION;
	//memcpy(&inst->cli.buff[1], = notif_uid;
	inst->cli.buff[5] = action_id;

	inst->cli.write_params.data = inst->cli.buff;
	inst->cli.write_params.length = 6;
	inst->cli.write_params.handle = inst->cli.control_point_handle;
	inst->cli.write_params.func = NULL;//TODO:

	return bt_gatt_write(inst->cli.conn, &inst->cli.write_params);
}

static uint8_t notification_source_notify(struct bt_conn* conn,
					  struct bt_gatt_subscribe_params* params,
					  const void* data, uint16_t length)
{
	const uint8_t* bytes = data;
	uint32_t notification_id;
	printk("EventID: %u, EventFlags: %u, CategoryID: %u, CategoryCount: %u, NotificationUID: %u",
		   bytes[0], bytes[1], bytes[2], bytes[3], bytes[4]);
/*	for (uint16_t i = 4; i < length; ++i) {
		printk("%c", bytes[i]);
	}*/
	memcpy(&notification_id, &bytes[4], sizeof(notification_id));

	BT_DBG("%s", event_id_str[bytes[0]]);
	BT_DBG("%s", category_id_str[bytes[2]]);
	BT_DBG("%d", notification_id);
	printk("\n");
	return BT_GATT_ITER_CONTINUE;
}

static uint8_t data_source_notify(struct bt_conn* conn,
				  struct bt_gatt_subscribe_params* params,
				  const void* data, uint16_t length)
{
#if 0
	const uint8_t* bytes = data;
	uint32_t notification_id;
	printk("EventID: %u, EventFlags: %u, CategoryID: %u, CategoryCount: %u, NotificationUID: %u",
		   bytes[0], bytes[1], bytes[2], bytes[3], bytes[4]);
/*	for (uint16_t i = 4; i < length; ++i) {
		printk("%c", bytes[i]);
	}*/
	memcpy(&notification_id, &bytes[4], sizeof(notification_id));

	BT_DBG("%s", event_id_str[bytes[0]]);
	BT_DBG("%s", category_id_str[bytes[2]]);
	BT_DBG("%d", notification_id);
#endif
	BT_DBG("");
	return BT_GATT_ITER_CONTINUE;
}

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
		struct bt_gatt_subscribe_params *sub_params = NULL;

		BT_DBG("Discovered attribute - uuid: %s, handle: %u", bt_uuid_str(chrc->uuid), attr->handle);
		if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANCS_NOTIFICATION_SOURCE)) {
			BT_DBG("ANCS Notification Source");
			inst->cli.notification_soure_handle = bt_gatt_attr_value_handle(attr);

			sub_params = &inst->cli.notification_source_subscribe_parms;

			sub_params->value = BT_GATT_CCC_NOTIFY;
			sub_params->notify = notification_source_notify;
			sub_params->value_handle = bt_gatt_attr_value_handle(attr);
			sub_params->ccc_handle = attr->handle + 2;
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANCS_CTRL_POINT)) {
			inst->cli.control_point_handle = bt_gatt_attr_value_handle(attr);
			BT_DBG("ANCS Control Point");
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_ANCS_DATA_SOURCE)) {
			inst->cli.data_soure_handle = bt_gatt_attr_value_handle(attr);
			BT_DBG("ANCS Data Source");

			sub_params = &inst->cli.data_source_subscribe_parms;

			sub_params->value = BT_GATT_CCC_NOTIFY;
			sub_params->notify = data_source_notify;
			sub_params->value_handle = bt_gatt_attr_value_handle(attr);
			sub_params->ccc_handle = attr->handle + 2;
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
