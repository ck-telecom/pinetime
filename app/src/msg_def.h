#ifndef _MSG_DEF_H
#define _MSG_DEF_H

#include <zephyr.h>

enum msg_type {
	MSG_TYPE_BLE_CONNECTED,
	MSG_TYPE_BLE_DISCONNECTED,
	MSG_TYPE_ACCEL_RAW,
	MSG_TYPE_BTN_DOWN,
	MSG_TYPE_BTN_UP,
	MSG_TYPE_BTN_LONG_PRESSED,
	MSG_TYPE_BLE_PASSKEY,
	MSG_TYPE_BLE_PAIRING_END,
};

struct msg_ble_conn {
	int err;
	struct bt_conn *conn;
};


void sys_push_msg(uint32_t msg_type);

void sys_push_msg_with_data(uint32_t msg_type, void *data, size_t size);

#endif /* _MSG_TYPE_H */
