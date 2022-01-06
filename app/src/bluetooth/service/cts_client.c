
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>

static uint8_t cts_client_read_cb(struct bt_conn *conn, uint8_t err,
			   struct bt_gatt_read_params *params,
			   const void *data, uint16_t length)
{
	LOG_DBG("Reading CCC data: err %d, %d bytes", err, length);

	if (err) {

	} else if (data) {
		if (length ==  10) {
			memcpy(&datetime_buf, data, length);
		}
	} else {

	}
	/* callback to notify? */
	return BT_GATT_ITER_CONTINUE;
}

int bt_cts_client_time_get()
{
	int err = 0;

	read_params.func = cts_client_read_cb;
	read_params.by_uuid.uuid = &uuid;
	read_params.by_uuid.start_handle = attr->handle;
	read_params.by_uuid.end_handle = 0xffff;

	err = bt_gatt_read(conn, &read_params);
	if (err < 0) {
		LOG_WRN("Could get time");
	}

	return err;
}

static uint8_t aics_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				  struct bt_gatt_discover_params *params)
{
	if (!attr) {
		LOG_DBG("CTS Service Discovery completed");
		return BT_GATT_ITER_STOP;
	}
	LOG_DBG("Discovered attribute, handle: %u\n", attr->handle);

	if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		if (!bt_uuid_cmp(params->uuid, BT_UUID_CTS_CURRENT_TIME) {

		}
	}
}

int bt_cts_discover(struct bt_conn *conn, struct bt_aics *inst,
		    const struct bt_aics_discover_param *param)
{
	int err = 0;

	return BT_GATT_ITER_CONTINUE;
}