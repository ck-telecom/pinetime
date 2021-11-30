#include <kernel.h>
#include <sys/slist.h>

#include <stdbool.h>

#include "event_service.h"
#include "../service/display.h"

#define EVENT_MALLOC
#define EVENT_FREE

typedef struct event_service_subscriber {
    EventServiceCommand command;
    EventServiceProc callback;
    void *context;
    k_tid_t thread; //NOTE: lvgl is not thread safe, so this thread is always in lvgl thread, and we don't need locker
    sys_snode_t node;
} event_service_subscriber;

static sys_slist_t event_list_head = SYS_SLIST_STATIC_INIT(&event_list_head);

void event_service_subscribe(EventServiceCommand command, EventServiceProc callback)
{
    event_service_subscriber *conn = app_malloc(1, sizeof(event_service_subscriber));
    conn->thread = k_current_get();
    conn->callback = callback;
    conn->command = command;

    sys_slist_append(&event_list_head, &conn->node);
}

void event_service_subscribe_with_context(EventServiceCommand command, EventServiceProc callback, void *context)
{
    event_service_subscriber *conn = app_malloc(1, sizeof(event_service_subscriber));
    conn->thread = k_current_get();
    conn->callback = callback;
    conn->command = command;
    conn->context = context;

    sys_slist_append(&event_list_head, &conn->node);;
}

void event_service_unsubscribe_thread(EventServiceCommand command, k_tid_t thread)
{
    event_service_subscriber *conn;
    SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
        if (conn->thread == thread)
        {
            sys_slist_find_and_remove(&event_list_head, &conn->node);
            app_free(conn);
            break;
        }
    }
}

void event_service_unsubscribe_thread_all(k_tid_t thread)
{
    event_service_subscriber *conn;
    SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
        if (conn->thread == thread)
        {
            sys_slist_find_and_remove(&event_list_head, &conn->node);
            app_free(conn);
            break;
        }
    }
}

void event_service_unsubscribe(EventServiceCommand command)
{
    k_tid_t _this_thread = k_current_get();
    event_service_unsubscribe_thread(command, _this_thread);
}

void *event_service_get_context(EventServiceCommand command)
{
    k_tid_t _this_thread = k_current_get();
    event_service_subscriber *conn;
    SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
        if (conn->thread == _this_thread && conn->command == command)
        {
            return conn->context;
        }
    }
    return NULL;
}

void event_service_set_context(EventServiceCommand command, void *context)
{
    k_tid_t _this_thread = k_current_get();
    event_service_subscriber *conn;
    SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, coon, node) {
        if (conn->thread == _this_thread && conn->command == command)
        {
            conn->context = context;
            return;
        }
    }
}

boolean event_service_post(EventServiceCommand command, void *data, DestroyEventProc destroy_callback)
{
    /* Step 1. post to the app thread */
    struct msg m;
    msg_send_data(&m, MSG_TYPE_EVT, (void *)data);
    if (appmanager_post_event_message(command, data, destroy_callback) == false)
    {
        //LOG_ERROR("Queue Full! Not processing");
        destroy_callback(data);
        return false;
    }

    return true;
}

void event_service_event_trigger(EventServiceCommand command, void *data, DestroyEventProc destroy_callback)
{
    k_tid_t _this_thread = k_current_get();
    event_service_subscriber *conn;
    SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, coon, node) {
        if (conn->command == command)
        {
//             LOG_INFO("Triggering %x %x %x", data, destroy, conn->callback);
            if (conn->thread == _this_thread && conn->callback)
                conn->callback(command, data, conn->context);
        }
    }
}