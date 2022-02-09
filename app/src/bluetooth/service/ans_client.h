#ifndef _ANS_CLIENT_H
#define _ANS_CLIENT_H

#include <bluetooth/uuid.h>

/** @def BT_UUID_ANS_VAL
 *  @brief Alert Nofification Service UUID value
 */
#define BT_UUID_ANS_VAL 0x1811

/** @def BT_UUID_ANS
 *  @brief Alert Nofification Service
 */
#define BT_UUID_ANS \
	BT_UUID_DECLARE_16(BT_UUID_ANS_VAL)

#define BT_UUID_ANS_ALERT_NOTIFICATION_CONTROL_POINT_VAL 0x2A44

#define BT_UUID_ANS_ALERT_NOTIFICATION_CONTROL_POINT \
	BT_UUID_DECLARE_16(BT_UUID_ANS_ALERT_NOTIFICATION_CONTROL_POINT_VAL)

#define BT_UUID_ANS_UNREAD_ALERT_STATUS_VAL 0x2A45

#define BT_UUID_ANS_UNREAD_ALERT_STATUS \
	BT_UUID_DECLARE_16(BT_UUID_ANS_UNREAD_ALERT_STATUS_VAL)

#define BT_UUID_ANS_NEW_ALERT_VAL 0x2A46

#define BT_UUID_ANS_NEW_ALERT \
	BT_UUID_DECLARE_16(BT_UUID_ANS_NEW_ALERT_VAL)

#define BT_UUID_ANS_SUPPORTED_NEW_ALERT_CATEGORY_VAL 0x2A47

#define BT_UUID_ANS_SUPPORTED_NEW_ALERT_CATEGORY \
	BT_UUID_DECLARE_16(BT_UUID_ANS_SUPPORTED_NEW_ALERT_CATEGORY_VAL)

#define BT_UUID_ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_VAL 0x2A48

#define BT_UUID_ANS_SUPPORTED_UNREAD_ALERT_CATEGORY \
	BT_UUID_DECLARE_16(BT_UUID_ANS_SUPPORTED_UNREAD_ALERT_CATEGORY_VAL)

struct bt_ans;

typedef void (*bt_ans_discover_cb)(struct bt_ans *inst, int err);

struct bt_ans_cb {
	bt_ans_discover_cb discover;
};

struct bt_ans_client {
	struct bt_gatt_write_params write_params;
	struct bt_gatt_read_params read_params;
	struct bt_gatt_discover_params discover_params;
	struct bt_gatt_subscribe_params notification_source_subscribe_parms;
	struct bt_gatt_subscribe_params data_source_subscribe_parms;
	struct bt_uuid_16 uuid;
	struct bt_conn *conn;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t notification_soure_handle;
	uint16_t control_point_handle;
	uint16_t data_soure_handle;
	uint8_t buff[32];
	bool busy;
	struct bt_ans_cb *cb;
};

struct bt_ans {

	union {
		struct bt_ans_client cli;
	};
};

#endif /* _ANS_CLIENT_H */