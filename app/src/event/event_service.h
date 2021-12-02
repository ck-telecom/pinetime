#ifndef _EVENT_SERVICE_H
#define _EVENT_SERVICE_H

typedef enum EventServiceCommand {
    EventServiceCommandGeneric,
    EventServiceCommandConnectionService,
    EventServiceCommandCall,
    EventServiceCommandMusic,
    EventServiceCommandNotification,
    EventServiceCommandAlert,
    EventServiceCommandProgress,
    EventServiceCommandBluetoothPairRequest,
} EventServiceCommand;

typedef void (*EventServiceProc)(EventServiceCommand command, void *data, void *context);

void event_service_subscribe(EventServiceCommand command, EventServiceProc callback);

void event_service_subscribe_with_context(EventServiceCommand command, EventServiceProc callback, void *context);

void event_service_unsubscribe_thread(EventServiceCommand command, k_tid_t thread);

void event_service_unsubscribe_thread_all(k_tid_t thread);

void event_service_unsubscribe(EventServiceCommand command);

void *event_service_get_context(EventServiceCommand command);

void event_service_set_context(EventServiceCommand command, void *context);

bool event_service_post(EventServiceCommand command, void *data/*, DestroyEventProc destroy_callback*/);

void event_service_event_trigger(EventServiceCommand command, void *data/*, DestroyEventProc destroy_callback*/);

#endif /* _EVENT_SERVICE_H */