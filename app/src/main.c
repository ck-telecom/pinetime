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

#include "msg_def.h"

#define BT_DISCOVER_DELAY 1 /* in s */

struct bt_cts cts_inst;
struct bt_ams ams_inst;
struct bt_ancs ancs_inst;

static struct bt_conn *default_conn;

K_MBOX_DEFINE(main_mailbox);

K_SEM_DEFINE(cts_sem, 0, 1);
K_SEM_DEFINE(ams_sem, 0, 1);
K_SEM_DEFINE(ancs_sem, 0, 1);

static uint8_t main_buffer[1024];

static const struct bt_data advertising_data[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

void sys_push_msg(uint32_t msg_type)
{
	struct k_mbox_msg send_msg;

	send_msg.info = msg_type;
	send_msg.size = 0;
	send_msg.tx_data = NULL;
	send_msg.tx_block.data = NULL;
	send_msg.tx_target_thread = K_ANY;

	k_mbox_async_put(&main_mailbox, &send_msg, NULL);
}

void sys_push_msg_with_data(uint32_t msg_type, void *data, int size)
{
}

int cts_start_discover(struct bt_conn *conn, uint8_t timeout)
{
	int r = 0;

	r = bt_cts_discover(conn, &cts_inst);
	k_sem_take(&cts_sem, K_SECONDS(timeout));

	return r;
}

static void cts_discover_cb(struct bt_cts *inst, int err)
{
	if (err) {
		BT_DBG("ancs discover failed");
	}
	k_sem_give(&cts_sem);
}

struct bt_cts_cb bt_cts_callbacks = {
	.discover = cts_discover_cb,
};

int ams_start_discover(struct bt_conn *conn, uint8_t timeout)
{
	int r = 0;

	r = bt_ams_discover(conn, &ams_inst);
	k_sem_take(&ams_sem, K_SECONDS(timeout));

	return r;
}

static void ams_discover_cb(struct bt_ams *inst, int err)
{
	if (err) {
		BT_DBG("ams discover failed");
	}
	k_sem_give(&ams_sem);
}

struct bt_ams_cb bt_ams_callbacks = {
	.discover = ams_discover_cb,
};

int ancs_start_discover(struct bt_conn *conn, uint8_t timeout)
{
	int r = 0;

	r = bt_ancs_discover(conn, &ancs_inst);
	k_sem_take(&ancs_sem, K_SECONDS(timeout));

	return r;
}

static void ancs_discover_cb(struct bt_ancs *inst, int err)
{
	if (err) {
		BT_DBG("ancs discover failed");
	}
	k_sem_give(&ancs_sem);
}

struct bt_ancs_cb bt_ancs_callbacks = {
	.discover = ancs_discover_cb,
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Failed to connect %d\n", err);
		return;
	}
	if (!default_conn) {
		default_conn = bt_conn_ref(conn);
	}
	sys_push_msg(MSG_TYPE_BLE_CONNECTED);
	printk("Connected\n");
}

static void disconnected(struct bt_conn* conn, uint8_t err)
{
	if (default_conn == conn) {
		bt_conn_unref(default_conn);
		default_conn = NULL;
	}
	sys_push_msg(MSG_TYPE_BLE_DISCONNECTED);
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

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Passkey for %s: %06u\n", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing completeed: %s\n", bonded ? "true" : "false");
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
	.pairing_complete = auth_pairing_complete,
};

void main(void)
{
	struct k_mbox_msg recv_msg;
	printk("Hello World! %s\n", CONFIG_BOARD);

	int error = bt_enable(NULL);
		if (error) {
		printk("Bluetooth failed to enable (err %d)\n", error);
		return;
	}

	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);

	error = bt_le_adv_start(BT_LE_ADV_CONN_NAME,
				advertising_data, ARRAY_SIZE(advertising_data),
				NULL, 0);
	if (error) {
		printk("Advertising failed to start (err %d)\n", error);
		return;
	}

	default_conn = NULL;

	bt_cts_client_cb_register(&cts_inst, &bt_cts_callbacks);
	bt_ams_client_cb_register(&ams_inst, &bt_ams_callbacks);
	bt_ancs_client_cb_register(&ancs_inst, &bt_ancs_callbacks);

	while (1) {
		recv_msg.info = -1;
		recv_msg.info = 1024;
		recv_msg.rx_source_thread = K_ANY;

		error = k_mbox_get(&main_mailbox, &recv_msg, main_buffer, K_FOREVER);

		switch (recv_msg.info) {
		case MSG_TYPE_BLE_CONNECTED:
			cts_start_discover(default_conn, BT_DISCOVER_DELAY);
			ams_start_discover(default_conn, BT_DISCOVER_DELAY);
			ancs_start_discover(default_conn, BT_DISCOVER_DELAY);

			printk("MSG_TYPE_BLE_CONNECTED Done\n");
			break;

		case MSG_TYPE_BLE_DISCONNECTED:
			//cts_client_reset();
			//ams_client_reset();
			//ancs_client_reset();
			printk("MSG_TYPE_BLE_DISCONNECTED Done\n");
			break;

		case MSG_TYPE_ACCEL_RAW:
			break;

		default:
			break;
		}

	}
}
