#include <zephyr.h>
#include <drivers/gpio.h>

#include <logging/log.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>

//#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(BT_APP, LOG_LEVEL_INF);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);
static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param);
static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);

static struct k_work advertise_work;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    /* Device information */
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
              0x0a, 0x18),
    /* Current time */
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
              0x05, 0x18),
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

    k_work_init(&advertise_work, advertise);
    k_work_submit(&advertise_work);
    LOG_INF("Bluetooth initialized");

    return err;
}

SYS_INIT(bt_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);