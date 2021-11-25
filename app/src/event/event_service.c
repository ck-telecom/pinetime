#include <kernel.h>

#include "event_service.h"

typedef struct event_service_subscriber {
    EventServiceCommand command;
    EventServiceProc callback;
    void *context;
    //app_running_thread *thread;
    k_tid_t thread; //NOTE: lvgl is not thread safe, so this thread is always in lvgl thread
    list_node node;
} event_service_subscriber;

void event_service_subscribe(EventServiceCommand command, EventServiceProc callback)
{
    app_running_thread *_this_thread = appmanager_get_current_thread();
    event_service_subscriber *conn = app_calloc(1, sizeof(event_service_subscriber));
    conn->thread = k_current_get();
    conn->callback = callback;
    conn->command = command;

    list_init_node(&conn->node);
    list_insert_head(&_subscriber_list_head, &conn->node);
}

void event_service_unsubscribe_thread(EventServiceCommand command, k_tid_t thread)
{
    event_service_subscriber *conn;
    list_foreach(conn, &_subscriber_list_head, event_service_subscriber, node)
    {
        if (conn->thread == thread)
        {
            list_remove(&_subscriber_list_head, &conn->node);
            remote_free(conn);
            break;
        }
    }
}

void event_service_unsubscribe_thread_all(k_tid_t thread)
{
    event_service_subscriber *conn;
    list_foreach(conn, &_subscriber_list_head, event_service_subscriber, node)
    {
        if (conn->thread == thread)
        {
            list_remove(&_subscriber_list_head, &conn->node);
            remote_free(conn);
            break;
        }
    }
}

void event_service_unsubscribe(EventServiceCommand command)
{
    k_tid_t _this_thread = k_current_get();
    event_service_unsubscribe_thread(command, _this_thread);
}

void event_service_event_trigger(EventServiceCommand command, void *data, DestroyEventProc destroy_callback)
{
    DestroyEventProc destroy = destroy_callback;

    if (destroy_callback)
        destroy = MK_THUMB_CB(destroy_callback);

    k_tid_t _this_thread = k_current_get();
    event_service_subscriber *conn;
    list_foreach(conn, &_subscriber_list_head, event_service_subscriber, node)
    {
        if (conn->command == command)
        {
//             LOG_INFO("Triggering %x %x %x", data, destroy, conn->callback);
            if (conn->thread == _this_thread && conn->callback)
                conn->callback(command, data, conn->context);

            /* Step 2. App processing done, post it to overlay thread */
            if (_this_thread->thread_type == AppThreadMainApp)
                overlay_window_post_event(command, data, destroy);

            /* Step 3. if we are the overlay thread, then destroy this packet */
            else if (_this_thread->thread_type == AppThreadOverlay)
            {
                if (destroy)
                    destroy(data);

                appmanager_post_draw_message(1);
            }
        }
    }
}