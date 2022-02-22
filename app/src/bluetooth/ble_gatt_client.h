#ifndef _BLE_GATT_CLIENT_H
#define _BLE_GATT_CLIENT_H

#include <bluetooth/uuid.h>

enum {
	EVT_CONNECTED,
	EVT_DISCOVERYED,
	EVT_DISCOVERY_COMPLETED,
	EVT_DISCONNTED,
};

typedef int (*discover_func)(struct bt_conn *conn, void *context);

struct ble_gatt_client {
	discover_func discover;
	void *context;
};

void gatt_client_register(struct ble_gatt_client *client);

#endif /* _BLE_GATT_CLIENT_H */