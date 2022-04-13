#include "health_service.h"

#include "blobdb.h"

HealthValue health_service_sum(HealthMetric metric, time_t time_start, time_t time_end)
{
	rdb_select_result_list list_head;
	uint16_t offsetof = 0;
	uint16_t size = 0;
	long long sum = 0;

	switch (metric)
	{
	case HealthMetricStepCount:
		offsetof = 0;
		size = 4;
		break;

	default:
		offsetof = 0;
		size = 8;
		break;
	}

	struct rdb_selector selectors[] = {
		{ RDB_SELECTOR_OFFSET_KEY, sizeof(time_t), RDB_OP_GREATER, &time_start },
		{ RDB_SELECTOR_OFFSET_KEY, sizeof(time_t), RDB_OP_LESS, &time_end },
		{ offsetof, size, RDB_OP_RESULT },
		{ }
	};

	int n = rdb_select(it, &list_head, selectors);
	if (!n) {
		return 0;
	}

	struct rdb_select_result *res;

	rdb_select_result_foreach(res, list_head) {
		for (int i = 0; i < res->nres; i++) {
			sum += res->result[i];
		}
	}

	return sum;
}

HealthValue health_service_sum_today(HealthMetric metric)
{
	time_t time_start = time_start_of_today();
	time_t time_end = time(NULL);
	return health_service_sum(metric, time_start, time_end);
}

HealthValue health_service_sum_averaged(HealthMetric metric, time_t time_start, time_t time_end, HealthServiceTimeScope scope)
{
	long long sum = health_service_sum(metric, time_start, time_end);

	switch (scope) {
	case HealthServiceTimeScopeWeekly:
		n = 7;
		break;

	default:
		break;
	}

	return sum / n;
}

HealthServiceAccessibilityMask health_service_metric_accessible(HealthMetric metric, time_t time_start, time_t time_end)
{
	uint16_t offsetof = 0;
	uint16_t size = 0;
	long long sum = 0;

	switch (metric)
	{
	case HealthMetricStepCount:
		offsetof = 0;
		size = 4;
		break;

	default:
		offsetof = 0;
		size = 8;
		break;
	}

	struct rdb_selector selectors[] = {
		{ RDB_SELECTOR_OFFSET_KEY, sizeof(time_t), RDB_OP_GREATER, &time_start },
		{ RDB_SELECTOR_OFFSET_KEY, sizeof(time_t), RDB_OP_LESS, &time_end },
		{ offsetof, size, RDB_OP_RESULT },
		{ }
	};

	int n = rdb_select(it, &list_head, selectors);

	return (n == 0) ? HealthServiceAccessibilityMaskNotAvailable : HealthServiceAccessibilityMaskAvailable;
}

HealthServiceAccessibilityMask health_service_metric_averaged_accessible(HealthMetric metric, time_t time_start, time_t time_end, HealthServiceTimeScope scope)
{

}

bool health_service_events_subscribe(HealthEventHandler handler, void * context)
{

}