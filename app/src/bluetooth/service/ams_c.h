/**
 * @file ams_c.h
 * @author your name (you@domain.com)
 * @brief Apple Media Service Client
 * @version 0.1
 * @date 2022-01-06
 *
 * @copyright Copyright (c) 2022 <gouqs@hotmail.com>
 *
 */

#ifndef _AMS_C_H
#define _AMS_C_H

#include <bluetooth/uuid.h>

/** @def BT_UUID_AMS_VAL
 *  @brief Apple Media Service UUID value
 */
#define BT_UUID_AMS_VAL \
	BT_UUID_128_ENCODE(0x89D3502B, 0x0F36, 0x433A, 0x8EF4, 0xC502AD55F8DC)

/** @def BT_UUID_AMS
 *  @brief Apple Media Service
 */
#define BT_UUID_AMS \
	BT_UUID_DECLARE_128(BT_UUID_AMS_VAL)

#define BT_UUID_AMS_REMOTE_CMD_VAL \
	BT_UUID_128_ENCODE(0x9B3C81D8, 0x57B1, 0x4A8A, 0xB8DF, 0x0E56F7CA51C2)

#define BT_UUID_AMS_REMOTE_CMD \
	BT_UUID_DECLARE_128(BT_UUID_AMS_REMOTE_CMD_VAL)

#define BT_UUID_AMS_ENTITY_UPDATE_VAL \
	BT_UUID_128_ENCODE(0x2F7CABCE, 0x808D, 0x411F, 0x9A0C, 0xBB92BA96C102)

#define BT_UUID_AMS_ENTITY_UPDATE \
	BT_UUID_DECLARE_128(BT_UUID_AMS_ENTITY_UPDATE_VAL)

#define BT_UUID_ENTITY_ATTRIBUTE_VAL \
	BT_UUID_128_ENCODE(0xC6B2F38C, 0x23AB, 0x46D8, 0xA6AB, 0xA3A870BBD5D7)

#define BT_UUID_AMS_ENTITY_ATTR \
	BT_UUID_DECLARE_128(BT_UUID_ENTITY_ATTRIBUTE_VAL)

enum ams_entity_id {
    AMS_ENTITY_ID_PLAYER,
    AMS_ENTITY_ID_QUEUE,
    AMS_ENTITY_ID_TRACK,
};

enum ams_player_attribute_id {
    AMS_PLAYER_ATTRIBUTE_ID_NAME,
    AMS_PLAYER_ATTRIBUTE_ID_PLAYBACKINFO,
    AMS_PLAYER_ATTRIBUTE_ID_VOLUME,
};

enum ams_queue_attribute_id {
    AMS_QUEUE_ATTRIBUTE_ID_INDEX,
    AMS_QUEUE_ATTRIBUTE_ID_COUNT,
    AMS_QUEUE_ATTRIBUTE_ID_SHUFFLEMODE,
    AMS_QUEUE_ATTRIBUTE_ID_REPEATMODE,
};

enum ams_track_attribute_id {
    AMS_TRACK_ATTRIBUTE_ID_ARTIST,
    AMS_TRACK_ATTRIBUTE_ID_ALBUM,
    AMS_TRACK_ATTRIBUTE_ID_TITLE,
    AMS_TRACK_ATTRIBUTE_ID_DURATION,
};

struct bt_ams_client {
	struct bt_gatt_write_params write_params;
	struct bt_gatt_read_params read_params;
	struct bt_gatt_discover_params discover_params;
	struct bt_gatt_subscribe_params entity_update_sub_params;
	struct bt_uuid_128 uuid;
	struct bt_conn *conn;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t entity_write_handle;
	uint16_t entity_subscribe_handle;
	uint8_t entity_update_command[2];
};

struct bt_ams {


	union {
		struct bt_ams_client cli;
	};
};

int bt_ams_discover(struct bt_conn *conn, struct bt_ams *inst);

#endif /* _AMS_C_H */