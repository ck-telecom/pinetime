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

#include "msg_def.h"
//#include "app.h"
#include "bt.h"

K_MBOX_DEFINE(main_mailbox);

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

void sys_push_msg_with_data(uint32_t msg_type, void *data, size_t size)
{
	struct k_mbox_msg send_msg;

	send_msg.info = msg_type;
	send_msg.size = size;
	send_msg.tx_data = data;
	send_msg.tx_block.data = NULL;
	send_msg.tx_target_thread = K_ANY;

	k_mbox_async_put(&main_mailbox, &send_msg, NULL);
}

void main(void)
{
	struct k_mbox_msg recv_msg;
	printk("Hello World! %s\n", CONFIG_BOARD);
	int error = 0;

	error = app_bt_init();
	if (error) {
		printk("BT initial failed");
		return;
	}

	while (1) {
		recv_msg.info = -1;
		recv_msg.size = 1024;
		recv_msg.rx_source_thread = K_ANY;

		error = k_mbox_get(&main_mailbox, &recv_msg, main_buffer, K_FOREVER);

		switch (recv_msg.info) {
		case MSG_TYPE_BLE_CONNECTED:
			//app_push_msg(MSG_TYPE_BLE_CONNECTED);

			printk("MSG_TYPE_BLE_CONNECTED Done\n");
			break;

		case MSG_TYPE_BLE_DISCONNECTED:
			//app_push_msg(MSG_TYPE_BLE_DISCONNECTED);

			//cts_client_reset();
			//ams_client_reset();
			//ancs_client_reset();
			printk("MSG_TYPE_BLE_DISCONNECTED Done\n");
			break;

		case MSG_TYPE_BTN_DOWN:
		case MSG_TYPE_BTN_UP:
		case MSG_TYPE_BTN_LONG_PRESSED:
			//app_push_msg(recv_msg.info);
			break;

		case MSG_TYPE_ACCEL_RAW:
			break;

		default:
			break;
		}

	}
}
