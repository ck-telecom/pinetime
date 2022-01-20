#ifndef _CTS_INTERNAL_H
#define _CTS_INTERNAL_H

struct bt_cts_client {
	struct bt_gatt_write_params write_params;
	struct bt_gatt_read_params read_params;
	struct bt_gatt_discover_params discover_params;
	struct bt_uuid_16 uuid;
	struct bt_conn *conn;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t time_handle;
	uint8_t cts_buf[10];
};

struct bt_cts {


	union {
		struct bt_cts_client cli;
	};

};

int bt_cts_discover(struct bt_conn *conn, struct bt_cts *inst);

#endif /* _CTS_INTERNAL_H */
