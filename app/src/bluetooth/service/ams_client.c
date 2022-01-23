#include <bluetooth/gatt.h>

#include "ams_c.h"

#define LOG_MODULE_NAME bt_ams_client
#include "common/log.h"

int ams_client_get_entity_attr(struct bt_ams *inst,
			       enum ams_entity_id entity_id,
			       uint8_t attr_id)
{
	memset(inst, 0, sizeof(inst->cli.write_params));

	inst->cli.buf[0] = entity_id;
	inst->cli.buf[1] = attr_id;

	inst->cli.write_params.handle = inst->cli.entity_attr_handle;
	inst->cli.write_params.data = inst->cli.buf;
	inst->cli.write_params.length = 2;

	return bt_gatt_write(inst->cli.conn, &inst->cli.write_params);
}

int ams_client_write_remote_command(struct bt_ams *inst, enum ams_remote_command_id rcid)
{
	memset(inst, 0, sizeof(inst->cli.write_params));

	inst->cli.buf[0] = rcid;
	inst->cli.write_params.handle = inst->cli.remote_command_handle;
	inst->cli.write_params.data = inst->cli.buf;
	inst->cli.write_params.length = 1;

	return bt_gatt_write(inst->cli.conn, &inst->cli.write_params);
}

static uint8_t entity_update_notify(struct bt_conn* conn,
                                 struct bt_gatt_subscribe_params* params,
                                 const void* data, uint16_t length)
{
    const uint8_t* bytes = data;
    BT_DBG("EntityID: %u, AttributeID: %u, Flags: %u, Value: ", bytes[0],
           bytes[1], bytes[2]);
    for (uint16_t i = 3; i < length; ++i) {
        BT_DBG("%c", bytes[i]);
    }
    BT_DBG("\n");
    return BT_GATT_ITER_CONTINUE;
}

static void entity_update_write_response(struct bt_conn* conn, uint8_t err,
                                         struct bt_gatt_write_params* params)
{

}

int ams_client_entity_write(struct bt_ams *inst,
			    enum ams_entity_id entity_id,
			    enum ams_player_attribute_id attr_id)
{
	inst->cli.entity_update_command[0] = entity_id;
	inst->cli.entity_update_command[1] = attr_id;
	inst->cli.write_params.data = inst->cli.entity_update_command;
	inst->cli.write_params.length = 2;
	inst->cli.write_params.offset = 0;
	inst->cli.write_params.func = entity_update_write_response;
	inst->cli.write_params.handle = inst->cli.entity_write_handle;

	return bt_gatt_write(inst->cli.conn, &inst->cli.write_params);
}

static uint8_t ams_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				 struct bt_gatt_discover_params *params)
{
	struct bt_ams_client *client_inst = CONTAINER_OF(params,
							 struct bt_ams_client,
							 discover_params);

	struct bt_ams *inst = CONTAINER_OF(client_inst, struct bt_ams, cli);

	if (!attr) {
		BT_DBG("AMS Discovery completed");
		return BT_GATT_ITER_STOP;
	}

	if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		struct bt_gatt_chrc *chrc =(struct bt_gatt_chrc *)attr->user_data;
		struct bt_gatt_subscribe_params *sub_params = NULL;

		BT_DBG("Discovered attribute - uuid: %s, handle: %u", bt_uuid_str(chrc->uuid), attr->handle);
		if (!bt_uuid_cmp(chrc->uuid, BT_UUID_AMS_ENTITY_UPDATE)) {
			BT_DBG("AMS entity update");
			inst->cli.entity_write_handle = attr->handle;
			inst->cli.entity_subscribe_handle = attr->handle + 1;
			//sub_params = &inst->cli.entity_update_sub_params;
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_AMS_ENTITY_ATTR)) {
			BT_DBG("AMS entity attr");
			inst->cli.entity_attr_handle = attr->handle;
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_AMS_REMOTE_CMD)) {
			BT_DBG("AMS Remote Command");
			inst->cli.remote_command_handle = attr->handle;
		}
		if (sub_params) {
			sub_params->notify = entity_update_notify;
			sub_params->value = BT_GATT_CCC_NOTIFY;
			sub_params->value_handle = attr->handle + 1;
			//sub_params->ccc_handle = attr->handle + 2;
			bt_gatt_subscribe(conn, sub_params);
		}
	}

	return BT_GATT_ITER_CONTINUE;
}

static uint8_t primary_discover_func(struct bt_conn *conn,
				     const struct bt_gatt_attr *attr,
				     struct bt_gatt_discover_params *params)
{
	struct bt_ams_client *client_inst = CONTAINER_OF(params,
							 struct bt_ams_client,
							 discover_params);

	struct bt_ams *inst = CONTAINER_OF(client_inst, struct bt_ams, cli);

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
		inst->cli.discover_params.func = ams_discover_func;

		err = bt_gatt_discover(conn, &inst->cli.discover_params);
		if (err) {
			BT_DBG("Discover failed (err %d)", err);
		}
		return BT_GATT_ITER_STOP;
	}

	return BT_GATT_ITER_CONTINUE;
}

int bt_ams_discover(struct bt_conn *conn, struct bt_ams *inst)
{
	int err = 0;

	memcpy(&inst->cli.uuid, BT_UUID_AMS, sizeof(inst->cli.uuid));

	inst->cli.conn = conn;
	inst->cli.discover_params.func = primary_discover_func;
	inst->cli.discover_params.uuid = &inst->cli.uuid.uuid;
	inst->cli.discover_params.type = BT_GATT_DISCOVER_PRIMARY;
	inst->cli.discover_params.start_handle = BT_ATT_FIRST_ATTTRIBUTE_HANDLE;
	inst->cli.discover_params.end_handle = BT_ATT_LAST_ATTTRIBUTE_HANDLE;

	err = bt_gatt_discover(conn, &inst->cli.discover_params);

	return err;
}
