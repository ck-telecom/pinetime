#ifndef _DISPLAY_API_H
#define _DISPLAY_API_H

#include <stdbool.h>
#include <kernel.h>

#define MSG_TYPE_GESTURE    10
#define MSG_TYPE_BUTTON     11
#define MSG_SLEEP           12
#define MSG_WAKEUP          13
#define MSG_BLE_CONNECTION  14
#define MSG_TYPE_EVT        15
#define MSG_TYPE_CHARGER    16
#define MSG_TYPE_ACCEL_RAW  17

struct msg {
    unsigned long type;
    union {
        void *data;              /**< for MSG_TYPE_GUI */
        unsigned long event;     /**< for MSG_TYPE_BUTTON */
        unsigned long gesture;   /**< for MSG_TYPE_GESTURE */
    };
};

struct screen_context {
    bool display_on;
};

int msg_send_event(struct msg *m, unsigned long type, unsigned long event);

int msg_send_data(uint32_t type, size_t size, void *data);

int msg_send_gesture(uint32_t gesture);
int msg_send_button(struct msg *m, uint32_t btn_event);

extern struct view home;
extern struct view clock_face_view;
extern struct view date_settings;

#endif /* _DISPLAY_H */
