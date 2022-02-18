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

typedef void (*event_service_callback_t)(uint32_t command, void *data, void *context);

void event_service_subscribe(uint32_t command, event_service_callback_t callback);

void event_service_subscribe_with_context(uint32_t command, event_service_callback_t callback, void *context);

void event_service_unsubscribe_thread(uint32_t command, k_tid_t thread);

void event_service_unsubscribe_thread_all(k_tid_t thread);

void event_service_unsubscribe(uint32_t command);

void *event_service_get_context(uint32_t command);

void event_service_set_context(uint32_t command, void *context);

bool event_service_post(uint32_t command, void *data);

void event_service_event_trigger(uint32_t command, void *data);

#endif /* _EVENT_SERVICE_H */