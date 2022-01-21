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
	ANCS_Category_ID_Other,
	ANCS_Category_ID_IncomingCall,
	ANCS_Category_ID_MissedCall,
	ANCS_Category_ID_Voicemail,
	ANCS_Category_ID_Social,
	ANCS_Category_ID_Schedule,
	ANCS_Category_ID_Email,
	ANCS_Category_ID_News,
	ANCS_Category_ID_HealthAndFitness,
	ANCS_Category_ID_BusinessAndFinance,
	ANCS_Category_ID_Location,
	ANCS_Category_ID_Entertainment
};

enum ancs_event_id {
	ancs_EventIDNotificationAdded,
	ancs_EventIDNotificationModified,
	ancs_EventIDNotificationRemoved
};

#define EventFlagSilent		BIT(0)
#define EventFlagImportant	BIT(1)
#define EventFlagPreExisting	BIT(2)
#define EventFlagPositiveAction BIT(3)
#define EventFlagNegativeAction BIT(4)

struct bt_ancs_client {
	struct bt_gatt_write_params write_params;
	struct bt_gatt_read_params read_params;
	struct bt_gatt_discover_params discover_params;
	struct bt_uuid_128 uuid;
	struct bt_conn *conn;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t notification_soure_handle;
	uint16_t control_point_handle;
	uint16_t data_soure_handle;
	uint8_t entity_update_command[2];
};

struct bt_ancs {

	union {
		struct bt_ancs_client cli;
	};
};

#endif /* _ANCS_C_H */