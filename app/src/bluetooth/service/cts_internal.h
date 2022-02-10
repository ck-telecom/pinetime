#ifndef _CTS_INTERNAL_H
#define _CTS_INTERNAL_H


/** @def BT_UUID_CTS_LOCAL_TIME_INFOMATION_VAL
 *  @brief CTS Characteristic Local Time Information UUID value
 */
#define BT_UUID_CTS_LOCAL_TIME_INFOMATION_VAL 0x2a0f
/** @def BT_UUID_CTS_LOCAL_TIME_INFOMATION
 *  @brief CTS Characteristic Local Time Information
 */
#define BT_UUID_CTS_LOCAL_TIME_INFOMATION \
	BT_UUID_DECLARE_16(BT_UUID_CTS_LOCAL_TIME_INFOMATION_VAL)

struct bt_cts;

typedef void (*bt_cts_discover_cb)(struct bt_cts *inst, int err);

struct bt_cts_cb {
	bt_cts_discover_cb discover;
};

struct bt_cts_client {
	struct bt_gatt_write_params write_params;
	struct bt_gatt_read_params read_params;
	struct bt_gatt_discover_params discover_params;
	struct bt_gatt_subscribe_params subscribe_parms;
	struct bt_uuid_16 uuid;
	struct bt_conn *conn;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t current_time_handle;
	uint8_t cts_buf[10];
	bool busy;
	struct bt_cts_cb *cb;
};

struct bt_cts {


	union {
		struct bt_cts_client cli;
	};

};

int bt_cts_discover(struct bt_conn *conn, struct bt_cts *inst);

void bt_cts_client_cb_register(struct bt_cts *inst, struct bt_cts_cb *cb);

#endif /* _CTS_INTERNAL_H */
