#include <zephyr.h>
#include <init.h>

#include <logging/log.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/gap.h>
#include <settings/settings.h>

#include "bluetooth/gatt_dm.h"
#include "bt.h"

#include <time.h>

//#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(BT_APP, LOG_LEVEL_INF);
/*
static struct bt_cts_client cts_c;
static bool has_cts;

struct bt_ams_client ams_c;
static bool has_ams;

struct bt_ancs_client ancs_c;
static bool has_ancs;
*/
static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);
static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param);
static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);

static struct k_work advertise_work;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
};

#if defined(CONFIG_BT_SMP)
static void identity_resolved(struct bt_conn* conn, const bt_addr_le_t* rpa, const bt_addr_le_t* identity)
{
	printk("Identity resolved\n");
}
#endif

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
static void security_changed(struct bt_conn* conn, bt_security_t level,
			     enum bt_security_err err)
{
	printk("Security level changed\n");
}
#endif

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.le_param_req = le_param_req,
	.le_param_updated = le_param_updated,
#if defined(CONFIG_BT_SMP)
	.identity_resolved = identity_resolved,
#endif
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
	.security_changed = security_changed,
#endif
};

void bt_cts_read_callback(struct bt_cts_client *cts_c,
			  struct bt_cts_current_time *current_time,
			  int err)
{
	if (err == 0) {
		struct tm now;
		struct timespec time;
		now.tm_year = current_time->exact_time_256.year - 1900;
		now.tm_mon = current_time->exact_time_256.month - 1;
		now.tm_mday = current_time->exact_time_256.day;
		now.tm_hour = current_time->exact_time_256.hours;
		now.tm_min = current_time->exact_time_256.minutes;
		now.tm_sec = current_time->exact_time_256.seconds;

		time.tv_sec = mktime(&now);
		time.tv_nsec = 0;

		clock_settime(CLOCK_REALTIME, &time);
	}
}

static int settings_runtime_load(void)
{
#if defined(CONFIG_BT_DIS_SETTINGS)
#if defined(CONFIG_BT_DIS_MODEL)
	settings_runtime_set("bt/dis/model",
				 CONFIG_BT_DIS_MODEL,
				 sizeof(CONFIG_BT_DIS_MODEL));
#endif
#if defined(CONFIG_BT_DIS_MANUF)
	settings_runtime_set("bt/dis/manuf",
				 CONFIG_BT_DIS_MANUF,
				 sizeof(CONFIG_BT_DIS_MANUF));
#endif
#if defined(CONFIG_BT_DIS_SERIAL_NUMBER)
	settings_runtime_set("bt/dis/serial",
				 CONFIG_BT_DIS_SERIAL_NUMBER_STR,
				 sizeof(CONFIG_BT_DIS_SERIAL_NUMBER_STR));
#endif
#if defined(CONFIG_BT_DIS_SW_REV)
	settings_runtime_set("bt/dis/sw",
				 CONFIG_BT_DIS_SW_REV_STR,
				 sizeof(CONFIG_BT_DIS_SW_REV_STR));
#endif
#if defined(CONFIG_BT_DIS_FW_REV)
	settings_runtime_set("bt/dis/fw",
				 CONFIG_BT_DIS_FW_REV_STR,
				 sizeof(CONFIG_BT_DIS_FW_REV_STR));
#endif
#if defined(CONFIG_BT_DIS_HW_REV)
	settings_runtime_set("bt/dis/hw",
				 CONFIG_BT_DIS_HW_REV_STR,
				 sizeof(CONFIG_BT_DIS_HW_REV_STR));
#endif
#endif
	return 0;
}

static void advertise(struct k_work *work)
{
	int rc;

//	bt_le_adv_stop();

	rc = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (rc) {
		LOG_ERR("Advertising failed to start (rc %d)", rc);
		return;
	}

	LOG_INF("Advertising successfully started");
}

static void discover_all_completed(struct bt_gatt_dm *dm, void *ctx)
{
	int err;
	char uuid_str[37];
	struct bt_gatt_context *context = ctx;

	const struct bt_gatt_dm_attr *gatt_service_attr =
			bt_gatt_dm_service_get(dm);
	const struct bt_gatt_service_val *gatt_service =
			bt_gatt_dm_attr_service_val(gatt_service_attr);

	size_t attr_count = bt_gatt_dm_attr_cnt(dm);

	bt_uuid_to_str(gatt_service->uuid, uuid_str, sizeof(uuid_str));
	//printk("Found service %s\n", uuid_str);
	//printk("Attribute count: %d\n", attr_count);

	if (!bt_uuid_cmp(gatt_service->uuid, BT_UUID_CTS)) {
		err = bt_cts_handles_assign(dm, &context->cts_c);
		if (err == 0) {
			context->has_cts = true;
		}
	} else if (!bt_uuid_cmp(gatt_service->uuid, BT_UUID_AMS)) {
		err = bt_ams_handles_assign(dm, &context->ams_c);
		if (err == 0) {
			context->has_ams = true;
		}
	} else if (!bt_uuid_cmp(gatt_service->uuid, BT_UUID_ANCS)) {
		err = bt_ancs_handles_assign(dm, &context->ancs_c);
		if (err == 0) {
			context->has_ancs = true;
		}
	}

#if CONFIG_BT_GATT_DM_DATA_PRINT
	bt_gatt_dm_data_print(dm);
#endif
	bt_gatt_dm_data_release(dm);

	bt_gatt_dm_continue(dm, NULL);
}

static void discover_all_service_not_found(struct bt_conn *conn, void *ctx)
{
	printk("No more services\n");
	struct bt_gatt_context *context = ctx;
	if (context->has_cts) {
		bt_cts_read_current_time(&context->cts_c, bt_cts_read_callback);
	}
}

static void discover_all_error_found(struct bt_conn *conn, int err, void *ctx)
{
	printk("The discovery procedure failed, err %d\n", err);
}

static struct bt_gatt_dm_cb discover_all_cb = {
	.completed = discover_all_completed,
	.service_not_found = discover_all_service_not_found,
	.error_found = discover_all_error_found,
};

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
//	ble_ctx->passkey = passkey;
//	app_push_msg_with_data(MSG_TYPE_BLE_PASSKEY, &ble_ctx->passkey, sizeof(ble_ctx->passkey));
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
	//ble_ctx->bonded = bonded;
	//app_push_msg_with_data(MSG_TYPE_BLE_PAIRING_END, &ble_ctx->bonded, sizeof(bool));
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
	int err = bt_gatt_dm_start(conn, NULL, &discover_all_cb, NULL);
	if (err) {
		printk("Failed to start discovery (err %d)\n", err);
	}
#endif
	printk("Pairing completeed: %s\n", bonded ? "true" : "false");
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
};

struct bt_conn_auth_info_cb auth_info = {
	.pairing_complete = auth_pairing_complete,
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	int sec_err = 0;
	if (err) {
		return;
	}
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
	if (bt_conn_get_security(conn) < BT_SECURITY_L4) {
		LOG_INF("set security level");
		sec_err = bt_conn_set_security(conn, BT_SECURITY_L4);
		if (sec_err) {
			LOG_ERR("bt_conn_set_security error: %d", sec_err);
		}
	}
#else
	err = bt_gatt_dm_start(conn, NULL, &discover_all_cb, NULL);
	if (err) {
		printk("Failed to start discovery (err %d)\n", err);
	}
#endif
	LOG_INF("connected");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("disconnected (reason: %u)", reason);
}

static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param)
{
	return true;
}

static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
}

int app_bt_init(void)
{
	int err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return err;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		err = settings_load();
		if (err) {
			LOG_ERR("Settings load failed (err %d)", err);
		}
	}

	err = bt_conn_auth_cb_register(&auth_cb_display);
	if (err) {
		LOG_ERR("bt_conn_auth_cb_register error");
	}

	err = bt_conn_auth_info_cb_register(&auth_info);
	if (err) {
		LOG_ERR("bt_conn_auth_info_cb_register error");
	}

	k_work_init(&advertise_work, advertise);
	k_work_submit(&advertise_work);
	LOG_INF("Bluetooth initialized");

	return err;
}
