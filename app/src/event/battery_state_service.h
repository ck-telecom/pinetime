#ifndef _BATTERY_STATE_SERVICE_H
#define _BATTERY_STATE_SERVICE_H

typedef struct battery_charge_state {
	uint8_t charge_percent;
	bool is_charging;
	bool is_plugged;
} BatteryChargeState;

typedef void(*BatteryStateHandler)(BatteryChargeState charge);

void battery_state_service_subscribe(BatteryStateHandler handler);

void battery_state_service_unsubscribe(void);

BatteryChargeState battery_state_service_peek(void);

#endif /* _BATTERY_STATE_SERVICE_H */