#ifndef _HEALTH_SERVICE_H
#define _HEALTH_SERVICE_H

#include <time.h>

typedef enum HealthMetric {
	HealthMetricStepCount,
	HealthMetricActiveSeconds,
	HealthMetricWalkedDistanceMeters,
	HealthMetricSleepSeconds,
	HealthMetricSleepRestfulSeconds,
	HealthMetricRestingKCalories,
	HealthMetricActiveKCalories,
	HealthMetricHeartRateBPM,
	HealthMetricHeartRateRawBPM
} HealthMetric;

enum HealthActivity {
	HealthActivityNone,
	HealthActivitySleep,
	HealthActivityRestfulSleep,
	HealthActivityWalk,
	HealthActivityRun,
	HealthActivityOpenWorkout
};

typedef enum HealthServiceAccessibilityMask {
	HealthServiceAccessibilityMaskAvailable,
	HealthServiceAccessibilityMaskNoPermission,
	HealthServiceAccessibilityMaskNotSupported,
	HealthServiceAccessibilityMaskNotAvailable
} HealthServiceAccessibilityMask;

typedef enum HealthEventType {
	HealthEventSignificantUpdate,
	HealthEventMovementUpdate,
	HealthEventSleepUpdate,
	HealthEventMetricAlert,
	HealthEventHeartRateUpdate
} HealthEventType;

typedef enum HealthServiceTimeScope {
	HealthServiceTimeScopeOnce,
	HealthServiceTimeScopeWeekly,
	HealthServiceTimeScopeDailyWeekdayOrWeekend,
	HealthServiceTimeScopeDaily
} HealthServiceTimeScope;

typedef int32_t HealthValue;

typedef uint32_t HealthActivityMask;

typedef void (* HealthEventHandler)(HealthEventType event, void *context);

HealthValue health_service_sum(HealthMetric metric, time_t time_start, time_t time_end);

HealthValue health_service_sum_today(HealthMetric metric);

HealthValue health_service_sum_averaged(HealthMetric metric, time_t time_start, time_t time_end, HealthServiceTimeScope scope);

HealthActivityMask health_service_peek_current_activities(void);

void health_service_activities_iterate(HealthActivityMask activity_mask, time_t time_start, time_t time_end, HealthIterationDirection direction, HealthActivityIteratorCB callback, void * context);

HealthServiceAccessibilityMask health_service_metric_accessible(HealthMetric metric, time_t time_start, time_t time_end);

HealthServiceAccessibilityMask health_service_metric_averaged_accessible(HealthMetric metric, time_t time_start, time_t time_end, HealthServiceTimeScope scope);

bool health_service_events_subscribe(HealthEventHandler handler, void * context);

bool health_service_events_unsubscribe(void);

uint32_t health_service_get_minute_history(HealthMinuteData * minute_data, uint32_t max_records, time_t * time_start, time_t * time_end);

#endif /* _HEALTH_SERVICE_H */
