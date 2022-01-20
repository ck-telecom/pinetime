/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
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


struct bt_cts cts_inst;

static const struct bt_data advertising_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

// Apple Media Service: 89D3502B-0F36-433A-8EF4-C502AD55F8DC
static struct bt_uuid_128 ams_uuid =
    BT_UUID_INIT_128(0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xC5, 0xF4, 0x8E,
                     0x3A, 0x43, 0x36, 0x0F, 0x2B, 0x50, 0xD3, 0x89);

/* Remote Command: 9B3C81D8-57B1-4A8A-B8DF-0E56F7CA51C2 */
static struct bt_uuid_128 remote_command_uuid =
    BT_UUID_INIT_128(0xC2, 0x51, 0xCA, 0xF7, 0x56, 0x0E, 0xDF, 0xB8,
                     0x8A, 0x4A, 0xB1, 0x57, 0xD8, 0x81, 0x3C, 0x9B);

// Entity Update: 2F7CABCE-808D-411F-9A0C-BB92BA96C102
static struct bt_uuid_128 entity_update_uuid =
    BT_UUID_INIT_128(0x02, 0xC1, 0x96, 0xBA, 0x92, 0xBB, 0x0C, 0x9A,
                     0x1F, 0x41, 0x8D, 0x80, 0xCE, 0xAB, 0x7C, 0x2F);

/* Entity Attribute: C6B2F38C-23AB-46D8-A6AB-A3A870BBD5D7 */
static struct bt_uuid_128 entity_attribute_uuid =
    BT_UUID_INIT_128(0xD7, 0xD5, 0xBB, 0x70, 0xA8, 0xA3, 0xAB, 0xA6,
                     0xD8, 0x46, 0xAB, 0x23, 0x8C, 0xF3, 0xB2, 0xC6);

// Client Characteristic Configuration UUID
static struct bt_uuid_16 gatt_ccc_uuid = BT_UUID_INIT_16(BT_UUID_GATT_CCC_VAL);

static uint8_t discovered(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                       struct bt_gatt_discover_params* params);

static struct bt_gatt_discover_params discover_params;

static uint8_t entity_update_notify(struct bt_conn* conn,
                                 struct bt_gatt_subscribe_params* params,
                                 const void* data, uint16_t length);

static struct bt_gatt_subscribe_params entity_update_subscribe_params = {
    .notify = entity_update_notify,
    .value = BT_GATT_CCC_NOTIFY,
};

static uint8_t entity_update_notify(struct bt_conn* conn,
                                 struct bt_gatt_subscribe_params* params,
                                 const void* data, uint16_t length)
{
    const uint8_t* bytes = data;
    printk("EntityID: %u, AttributeID: %u, Flags: %u, Value: ", bytes[0],
           bytes[1], bytes[2]);
    for (uint16_t i = 3; i < length; ++i) {
        printk("%c", bytes[i]);
    }
    printk("\n");
    return BT_GATT_ITER_CONTINUE;
}

static uint8_t entity_update_command[2];

static void entity_update_write_response(struct bt_conn* conn, uint8_t err,
                                         struct bt_gatt_write_params* params)
{

}

static struct bt_gatt_write_params entity_update_write_params = {
    .data = entity_update_command,
    .func = entity_update_write_response,
    .length = 2,
    .offset = 0,
};

static uint8_t discovered(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                       struct bt_gatt_discover_params* params)
{
    if (!attr) {
        printk("Discover complete\n");
        memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }
    printk("Discovered attribute - uuid: %s, handle: %u\n", bt_uuid_str(params->uuid), attr->handle);
    if (bt_uuid_cmp(params->uuid, &ams_uuid.uuid) == 0) {
        struct bt_gatt_service_val* gatt_service = attr->user_data;
        discover_params.start_handle = attr->handle + 1;
        discover_params.end_handle = gatt_service->end_handle;
        discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
        discover_params.uuid = &entity_update_uuid.uuid;
        bt_gatt_discover(conn, &discover_params);
    } else if (bt_uuid_cmp(params->uuid, &remote_command_uuid.uuid) == 0) {

    } else if (bt_uuid_cmp(params->uuid, &entity_update_uuid.uuid) == 0) {
        entity_update_write_params.handle = attr->handle + 1;
        entity_update_subscribe_params.value_handle = attr->handle + 1;

        discover_params.start_handle = attr->handle + 2;
        discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
        discover_params.uuid = &gatt_ccc_uuid.uuid;
        bt_gatt_discover(conn, &discover_params);
    } else if (bt_uuid_cmp(params->uuid, &gatt_ccc_uuid.uuid) == 0) {
        entity_update_subscribe_params.ccc_handle = attr->handle;

        bt_gatt_subscribe(conn, &entity_update_subscribe_params);

        entity_update_command[0] = AMS_ENTITY_ID_PLAYER;
        entity_update_command[1] = AMS_PLAYER_ATTRIBUTE_ID_NAME;
        bt_gatt_write(conn, &entity_update_write_params);

        entity_update_command[0] = AMS_ENTITY_ID_PLAYER;
        entity_update_command[1] = AMS_PLAYER_ATTRIBUTE_ID_VOLUME;
        bt_gatt_write(conn, &entity_update_write_params);
    }
    return BT_GATT_ITER_STOP;
}

static void connected(struct bt_conn* conn, uint8_t err)
{
	int r = 0;

	if (err) {
		printk("Failed to connect\n");
		return;
	}
	printk("Connected\n");

	discover_params.func = discovered;
	discover_params.start_handle = 0x0001;
	discover_params.end_handle = 0xFFFF;
	discover_params.type = BT_GATT_DISCOVER_PRIMARY;
	discover_params.uuid = &ams_uuid.uuid;
	r = bt_gatt_discover(conn, &discover_params);
	if (r) {
		printk("discovery ams error %d", r);
	}

	r = bt_cts_discover(conn, &cts_inst);
	if (r) {
		printk("discovery cts error %d", r);
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
        k_sleep(K_MSEC(10));
    }
}
