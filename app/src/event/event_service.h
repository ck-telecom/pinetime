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

void event_service_subscribe(EventServiceCommand command, EventServiceProc callback);

void event_service_subscribe_with_context(EventServiceCommand command, EventServiceProc callback, void *context);

bool event_service_post(EventServiceCommand command, void *data, DestroyEventProc destroy_callback);

void event_service_event_trigger(EventServiceCommand command, void *data, DestroyEventProc destroy_callback);

#endif /* _EVENT_SERVICE_H */