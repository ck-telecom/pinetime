#include <zephyr.h>
#include <init.h>

#include <logging/log.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/gap.h>
#include <settings/settings.h>

//#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(BT_APP, LOG_LEVEL_INF);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);
static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param);
static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);

static struct k_work advertise_work;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
                  BT_UUID_16_ENCODE(BT_UUID_DIS_VAL),
                  BT_UUID_16_ENCODE(BT_UUID_CTS_CURRENT_TIME_VAL),
                 ),
#ifdef CONFIG_MCUMGR
    /* SMP */
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
              0x84, 0xaa, 0x60, 0x74, 0x52, 0x8a, 0x8b, 0x86,
              0xd3, 0x4c, 0xb7, 0x1d, 0x1d, 0xdc, 0x53, 0x8d),
#endif
};

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
    .le_param_req = le_param_req,
    .le_param_updated = le_param_updated,
};

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

static struct bt_gatt_discover_params cts_discovery_params;
static struct bt_uuid_16 uuid = BT_UUID_INIT_16(0);
static struct bt_gatt_read_params read_params;
int offset = 0;
//cts_datetime_t datetime_buf;

uint8_t cts_sync_read(struct bt_conn *conn, uint8_t err,
				    struct bt_gatt_read_params *params,
				    const void *data, uint16_t length)
{
    LOG_DBG("Reading CCC data: err %d, %d bytes, offset %d.", err, length, offset);

    if (!data || length <= 0) {
        //sync_cts_to_clock(&datetime_buf);
        return BT_GATT_ITER_STOP;
    }

    //memcpy(&datetime_buf + offset, data, length);
    offset += length;

    return BT_GATT_ITER_CONTINUE;
}

uint8_t cts_sync_service_discovered(struct bt_conn* conn, const struct bt_gatt_attr* attr,
        struct bt_gatt_discover_params* params)
{
    if (!attr) {
        LOG_DBG("CTS Service Discovery completed");
        return BT_GATT_ITER_STOP;
    }
    LOG_DBG("Discovered attribute, handle: %u\n", attr->handle);

    memset(&read_params, 0, sizeof(read_params));
    read_params.func = cts_sync_read;
    read_params.by_uuid.uuid = &uuid.uuid;
    read_params.by_uuid.start_handle = attr->handle;
    read_params.by_uuid.end_handle = 0xffff;
    offset = 0;
    if (bt_gatt_read(conn, &read_params) < 0) {
        LOG_WRN("Could not initiate read of CCC data.");
    }

    return BT_GATT_ITER_STOP;
}

static void cts_sync_processor(struct bt_conn *conn, void *data)
{
    memcpy(&uuid, BT_UUID_CTS_CURRENT_TIME, sizeof(uuid));
    cts_discovery_params.func = cts_sync_service_discovered;
    cts_discovery_params.start_handle = 0x0001;
    cts_discovery_params.end_handle = 0xFFFF;
    cts_discovery_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    cts_discovery_params.uuid = &uuid.uuid;

    if (bt_gatt_discover(conn, &cts_discovery_params) != 0) {
        LOG_ERR("CTS Sync > GATT discovery FAILED.\n");
    }
}

static void advertise(struct k_work *work)
{
    int rc;

    bt_le_adv_stop();

    rc = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (rc) {
        LOG_ERR("Advertising failed to start (rc %d)", rc);
        return;
    }

    LOG_INF("Advertising successfully started");
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        return;
    }

    LOG_INF("connected");
    // cts_sync_processor(conn, NULL);
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

static int bt_init(const struct device *dev)
{
    int err = bt_enable(NULL);
    if (err)
    {
        LOG_ERR("Bluetooth init failed (err %d)", err);
        return err;
    }

    err = settings_load();
    if (err) {
        LOG_ERR("Settings load failed (err %d)", err);
    }

    err = settings_runtime_load();
    if (err) {
        LOG_ERR("Settings runtime load failed (err %d)", err);
    }
    k_work_init(&advertise_work, advertise);
    k_work_submit(&advertise_work);
    LOG_INF("Bluetooth initialized");

    return err;
}

SYS_INIT(bt_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);