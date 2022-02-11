#ifndef _MSG_DEF_H
#define _MSG_DEF_H

enum msg_type {
	MSG_TYPE_BLE_CONNECTED,
	MSG_TYPE_BLE_DISCONNECTED,
	MSG_TYPE_ACCEL_RAW,
	MSG_TYPE_BTN_DOWN,
	MSG_TYPE_BTN_UP,
	MSG_TYPE_BTN_LONG_PRESSED,
};

struct msg_ble_conn {
	int err;
	struct bt_conn *conn;
};

#endif /* _MSG_TYPE_H */
