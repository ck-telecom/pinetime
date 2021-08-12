#ifndef _GUI_H
#define _GUI_H

#define MSG_TYPE_GESTURE 0

struct msg {
    uint32_t type;
    union {
        void *data;          /**< for MSG_TYPE_GUI */
        uint32_t event;     /**< for MSG_TYPE_EVENT */
        uint32_t gesture;   /**< for MSG_TYPE_GESTURE */
    };
};

int msg_send_event(struct msg *m, uint32_t event);

int msg_send_data(struct msg *m, uint32_t type, void *data);

int msg_send_gesture(struct msg *m, uint32_t gesture);

#endif /* _GUI_H */
