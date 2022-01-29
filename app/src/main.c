/*
 * Copyright (c) 2022 Qingsong Gou <gouqs@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <common/log.h>

#include "cts_internal.h"
#include "ams_c.h"
#include "ancs_c.h"

struct bt_cts cts_inst;
struct bt_ams ams_inst;
struct bt_ancs ancs_inst;

static const struct bt_data advertising_data[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

static void connected(struct bt_conn* conn, uint8_t err)
{
	int r = 0;

	if (err) {
		printk("Failed to connect\n");
		return;
	}
	printk("Connected\n");

	r = bt_ams_discover(conn, &ams_inst);
	if (r) {
		printk("discovery ams error %d", r);
	}

	r = bt_cts_discover(conn, &cts_inst);
	if (r) {
		printk("discovery cts error %d", r);
	}

	r = bt_ancs_discover(conn, &ancs_inst);
	if (r) {
		printk("discovery ancs error %d", r);
	}
}

static void disconnected(struct bt_conn* conn, uint8_t err)
{
	printk("Disconnected\n");
}

#if defined(CONFIG_BT_SMP)
static void identity_resolved(struct bt_conn* conn, const bt_addr_le_t* rpa, const bt_addr_le_t* identity)
{
	printk("Identity resolved\n");
}
#endif

static void security_changed(struct bt_conn* conn, bt_security_t level,
			     enum bt_security_err err)
{
	printk("Security level changed\n");
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
#if defined(CONFIG_BT_SMP)
	.identity_resolved = identity_resolved,
#endif
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
	.security_changed = security_changed,
#endif
};

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	int error = bt_enable(NULL);
		if (error) {
		printk("Bluetooth failed to enable (err %d)\n", error);
		return;
	}

	bt_conn_cb_register(&conn_callbacks);

	error = bt_le_adv_start(BT_LE_ADV_CONN_NAME,
				advertising_data, ARRAY_SIZE(advertising_data),
				NULL, 0);
	if (error) {
		printk("Advertising failed to start (err %d)\n", error);
		return;
	}


	while (1)
	{
		k_sleep(K_MSEC(1000));
	}
}
