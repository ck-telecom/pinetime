#include <zephyr.h>
#include <sys/slist.h>

#include <stdbool.h>

#include "event_service.h"
#include "event_service_mem.h"

#include "msg_def.h"

#define EVENT_MALLOC
#define EVENT_FREE

typedef struct event_service_subscriber {
	uint32_t command;
	event_service_callback_t callback;
	void *context;
	k_tid_t thread; //NOTE: lvgl is not thread safe, so this thread is always in lvgl thread, and we don't need locker
	sys_snode_t node;
} event_service_subscriber_t;

K_MUTEX_DEFINE(event_list_mutex);

static sys_slist_t event_list_head = SYS_SLIST_STATIC_INIT(&event_list_head);
#if 1
void event_service_subscribe(uint32_t command, event_service_callback_t callback)
{
	event_service_subscriber_t *conn = app_alloc(sizeof(event_service_subscriber_t));
	conn->thread = k_current_get();
	conn->callback = callback;
	conn->command = command;

	k_mutex_lock(&event_list_mutex, K_FOREVER);

	sys_slist_append(&event_list_head, &conn->node);

	k_mutex_unlock(&event_list_mutex);
}

void event_service_subscribe_with_context(uint32_t command, event_service_callback_t callback, void *context)
{
	event_service_subscriber_t *conn = app_alloc(sizeof(event_service_subscriber_t));

	conn->thread = k_current_get();
	conn->callback = callback;
	conn->command = command;
	conn->context = context;

	k_mutex_lock(&event_list_mutex, K_FOREVER);

	sys_slist_append(&event_list_head, &conn->node);

	k_mutex_unlock(&event_list_mutex);
}

void event_service_unsubscribe_thread(uint32_t command, k_tid_t thread)
{
	event_service_subscriber_t *conn;

	SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
		if (conn->thread == thread) {
			sys_slist_find_and_remove(&event_list_head, &conn->node);
			app_free(conn);
			break;
		}
	}
}

void event_service_unsubscribe_thread_all(k_tid_t thread)
{
	event_service_subscriber_t *conn;
	SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
		if (conn->thread == thread) {
			sys_slist_find_and_remove(&event_list_head, &conn->node);
			app_free(conn);
			break;
		}
	}
}

void event_service_unsubscribe(uint32_t command)
{
	k_tid_t _this_thread = k_current_get();
	event_service_unsubscribe_thread(command, _this_thread);
}

void *event_service_get_context(uint32_t command)
{
	k_tid_t _this_thread = k_current_get();
	event_service_subscriber_t *conn;

	SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
		if (conn->thread == _this_thread && conn->command == command) {
			return conn->context;
		}
	}

	return NULL;
}

void event_service_set_context(uint32_t command, void *context)
{
	k_tid_t _this_thread = k_current_get();
	event_service_subscriber_t *conn;

	SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
		if (conn->thread == _this_thread && conn->command == command) {
			conn->context = context;
			return;
		}
	}
}

bool event_service_post(uint32_t command, void *data)
{
	//sys_push_msg_with_data(command, data);

	return true;
}

void event_service_event_trigger(uint32_t command, void *data)
{
	k_tid_t _this_thread = k_current_get();
	event_service_subscriber_t *conn;

	k_mutex_lock(&event_list_mutex, K_FOREVER);

	SYS_SLIST_FOR_EACH_CONTAINER(&event_list_head, conn, node) {
		if (conn->command == command) {
			//LOG_INFO("Triggering %x %x %x", data, destroy, conn->callback);
			if (conn->thread == _this_thread && conn->callback) {
				conn->callback(command, data, conn->context);
			}
		}
	}

	k_mutex_unlock(&event_list_mutex);
}
#endif