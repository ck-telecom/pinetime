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
#define BT_UUID_AMS_VAL

/** @def BT_UUID_AMS
 *  @brief Apple Media Service
 */
#define BT_UUID_AMS \
	BT_UUID_DECLARE_128(BT_UUID_AMS_VAL)


#define BT_UUID_AMS_ENTITY_UPDATE_VAL

#define BT_UUID_AMS_ENTITY_UPDATE
	BT_UUID_DECLARE_128(BT_UUID_AMS_ENTITY_UPDATE_VAL)

#define BT_UUID_ENTITY_ATTRIBUTE_VAL

#define BT_UUID_AMS_ENTITY_ATTR
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

#endif /* _AMS_C_H */