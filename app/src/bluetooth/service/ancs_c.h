#ifndef _ANCS_C_H
#define _ANCS_C_H

#include <bluetooth/uuid.h>

/** @def BT_UUID_ANCS_VAL
 * @brief Apple Notification Center Service UUID value
 */
#define BT_UUID_ANCS_VAL \
	BT_UUID_128_ENCODE(0x7905F431, 0xB5CE, 0x4E99, 0xA40F, 0x4B1E122D00D0)

/** @def BT_UUID_ANCS
 *  @brief Apple Notification Center Service
 */
#define BT_UUID_ANCS \
	BT_UUID_DECLARE_128(BT_UUID_ANCS_VAL)

#define BT_UUID_ANCS_NOTIFICATION_SOURCE_VAL \
	BT_UUID_128_ENCODE(0x9FBF120D, 0x6301, 0x42D9, 0x8C58, 0x25E699A21DBD)

#define BT_UUID_ANCS_NOTIFICATION_SOURCE \
	BT_UUID_DECLARE_128(BT_UUID_ANCS_NOTIFICATION_SOURCE_VAL)

#define BT_UUID_ANCS_CTRL_POINT_VAL \
	BT_UUID_128_ENCODE(0x69D1D8F3, 0x45E1, 0x49A8, 0x9821, 0x9BBDFDAAD9D9)

#define BT_UUID_ANCS_CTRL_POINT \
	BT_UUID_DECLARE_128(BT_UUID_ANCS_CTRL_POINT_VAL)

#define BT_UUID_ANCS_DATA_SOURCE_VAL \
	BT_UUID_128_ENCODE(0x22EAC6E9, 0x24D6, 0x4BB5, 0xBE44, 0xB36ACE7C7BFB)

#define BT_UUID_ANCS_DATA_SOURCE \
	BT_UUID_DECLARE_128(BT_UUID_ANCS_DATA_SOURCE_VAL)

enum ancs_category_id {
	ANCS_CATEGORY_ID_OTHER,
	ANCS_CATEGORY_ID_INCOMING_CALL,
	ANCS_CATEGORY_ID_MISSED_CALL,
	ANCS_CATEGORY_ID_VOIC_EMAIL,
	ANCS_CATEGORY_ID_SOCIAL,
	ANCS_CATEGORY_ID_SCHEDULE,
	ANCS_CATEGORY_ID_EMAIL,
	ANCS_CATEGORY_ID_NEWS,
	ANCS_CATEGORY_ID_HEALTH_AND_FITNESS,
	ANCS_CATEGORY_ID_BUSINESS_AND_FINANCE,
	ANCS_CATEGORY_ID_LOCATION,
	ANCS_CATEGORY_ID_ENTERTAINMENT,
	ANCS_CATEGORY_ID_MAX
};

enum ancs_event_id {
	ANCS_EVENT_ID_NOTIFICATION_ADDED,
	ANCS_EVENT_ID_NOTIFICATION_MODIFIED,
	ANCS_EVENT_ID_NOTIFICATION_REMOVED,
	ANCS_EVENT_ID_MAX
};

enum ancs_command_id {
	ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES,
	ANCS_COMMAND_ID_GET_APP_ATTRIBUTES,
	ANCS_COMMAND_ID_GET_PERFORM_NOTIF_ACTION,
	ANCS_COMMAND_ID_MAX
};

enum ancs_notif_attr_id {
	ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER,
	ANCS_NOTIF_ATTR_ID_TITLE,
	ANCS_NOTIF_ATTR_ID_SUBTITLE,
	ANCS_NOTIF_ATTR_ID_MESSAGE,
	ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE,
	ANCS_NOTIF_ATTR_ID_DATE,
	ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL,
	ANCS_NOTIF_ATTR_ID_NEGATIVE_ACTION_LABEL
};

enum ble_ancs_c_action_id {
	ANCS_ACTION_ID_POSITIVE = 0,
	ANCS_ACTION_ID_NEGATIVE
};

#define EventFlagSilent		BIT(0)
#define EventFlagImportant	BIT(1)
#define EventFlagPreExisting	BIT(2)
#define EventFlagPositiveAction BIT(3)
#define EventFlagNegativeAction BIT(4)

struct bt_ancs;

typedef void (*bt_ancs_discover_cb)(struct bt_ancs *inst, int err);

struct bt_ancs_cb {
	bt_ancs_discover_cb discover;
};

struct bt_ancs_client {
	struct bt_gatt_write_params write_params;
	struct bt_gatt_read_params read_params;
	struct bt_gatt_discover_params discover_params;
	struct bt_gatt_subscribe_params notification_source_subscribe_parms;
	struct bt_gatt_subscribe_params data_source_subscribe_parms;
	struct bt_uuid_128 uuid;
	struct bt_conn *conn;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t notification_soure_handle;
	uint16_t control_point_handle;
	uint16_t data_soure_handle;
	uint8_t buff[32];
	bool busy;
	struct bt_ancs_cb *cb;
};

struct bt_ancs {

	union {
		struct bt_ancs_client cli;
	};
};

int bt_ancs_discover(struct bt_conn *conn, struct bt_ancs *inst);

void bt_ancs_client_cb_register(struct bt_ancs *inst, struct bt_ancs_cb *cb);

#endif /* _ANCS_C_H */