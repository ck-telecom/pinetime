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

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
    .le_param_req = le_param_req,
    .le_param_updated = le_param_updated,
};

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

    LOG_DBG("Bluetooth initialized");

    return err;
}

SYS_INIT(bt_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);