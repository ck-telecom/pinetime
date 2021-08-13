#ifndef _GUI_H
#define _GUI_H

#define MSG_TYPE_GESTURE    0
#define MSG_TYPE_BUTTON     1

struct msg {
    uint32_t type;
    union {
        void *data;          /**< for MSG_TYPE_GUI */
        uint32_t event;     /**< for MSG_TYPE_BUTTON */
        uint32_t gesture;   /**< for MSG_TYPE_GESTURE */
    };
};

int msg_send_event(struct msg *m, uint32_t event);

int msg_send_data(struct msg *m, uint32_t type, void *data);

int msg_send_gesture(struct msg *m, uint32_t gesture);
int msg_send_button(struct msg *m, uint32_t btn_event);

extern struct view home;
extern struct view clocl_face_view;

#endif /* _GUI_H */
