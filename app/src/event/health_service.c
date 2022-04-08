#include "health_service.h"

HealthValue health_service_sum(HealthMetric metric, time_t time_start, time_t time_end)
{
	switch (metric)
	{
	case HealthMetricStepCount:
		/* code */
		break;

	default:
		break;
	}
}

HealthValue health_service_sum_today(HealthMetric metric)
{
	time_t time_start = time_start_of_today();
	time_t time_end = time(NULL);
	return health_service_sum(metric, time_start, time_end);
}

HealthValue health_service_sum_averaged(HealthMetric metric, time_t time_start, time_t time_end, HealthServiceTimeScope scope)
{
	
}

HealthServiceAccessibilityMask health_service_metric_accessible(HealthMetric metric, time_t time_start, time_t time_end)
{
	
}

HealthServiceAccessibilityMask health_service_metric_averaged_accessible(HealthMetric metric, time_t time_start, time_t time_end, HealthServiceTimeScope scope)
{
	
}

bool health_service_events_subscribe(HealthEventHandler handler, void * context)
{
	
}