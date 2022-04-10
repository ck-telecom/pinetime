#ifndef _APP_BT_H
#define _APP_BT_H

#include "bluetooth/services/cts_client.h"
#include "bluetooth/services/ams_client.h"
#include "bluetooth/services/ancs_client.h"

int app_bt_init(void);

struct bt_gatt_context {
	struct bt_cts_client cts_c;
	bool has_cts;

	struct bt_ams_client ams_c;
	bool has_ams;

	struct bt_ancs_client ancs_c;
	bool has_ancs;
};

#endif /* _APP_BT_H */