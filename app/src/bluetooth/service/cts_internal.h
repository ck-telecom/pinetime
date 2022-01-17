

struct bt_cts_client {
	struct bt_gatt_write_params write_params;
	struct bt_gatt_read_params read_params;
	struct bt_gatt_discover_params discover_params;
	struct bt_uuid_16 uuid;
	struct bt_conn *conn;
};

struct bt_cts {
	uint16_t time_handle;

	union {
		struct bt_cts_client cli;
	};

};