#include <bluetooth/uuid.h>

#include "ams_c.h"
#include "ancs_c.h"

struct ble_gatt_client cts_client = {
	.discover = bt_cts_discover;
	.context = &cts_inst;
};

void gatt_client_register(struct ble_gatt_client *client)
{

}

void gatt_client_event_handler(uint32_t event)
{
	struct ble_gatt_client *client;

	switch (event) {
	case EVT_CONNECTED:
		i = 0;
		/* start discovery */
		client->discover(gatt_client->conn, client->context);
		break;

	case EVT_DISCOVERYED:
		i++;
		client++;
		if (client) {
			client->discover(gatt_client->conn, client->context);
		} else {
			gatt_client_event_handler(EVT_DISCOVERY_COMPLETED);
		}
		break;

	case EVT_DISCOVERY_COMPLETED:
		break;

	case EVT_DISCONNTED:
		for (int i = 0; i < ARRAY_SIZE(gatt_client); i++) {
			gatt_client->services[i].reset();
		}
		break;

	default:
		break;
	}
}

static void cts_discover_cb(struct bt_cts *inst, int err)
{
	if (err) {
		BT_DBG("ancs discover failed");
	}
	gatt_client_event_handler(EVT_DISCOVERYED);
}

struct bt_cts_cb bt_cts_callbacks = {
	.discover = cts_discover_cb,
};

void gatt_client_init()
{
	gatt_client_register(&cts_client);
}
