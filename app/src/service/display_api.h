#ifndef _DISPLAY_API_H
#define _DISPLAY_API_H

#include <stdbool.h>

#define MSG_TYPE_GESTURE    0
#define MSG_TYPE_BUTTON     1
#define MSG_SLEEP           2
#define MSG_WAKEUP          3
#define MSG_BLE_CONNECTION  4
#define MSG_TYPE_EVT        5
#define MSG_TYPE_CHARGER    6

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

int msg_send_data(struct msg *m, uint32_t type, void *data);

int msg_send_gesture(uint32_t gesture);
int msg_send_button(struct msg *m, uint32_t btn_event);
int msg_send_data(struct msg *m, uint32_t type, void *data);

extern struct view home;
extern struct view clock_face_view;

#endif /* _DISPLAY_H */
