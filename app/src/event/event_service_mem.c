#include "event_service_mem.h"
#include <zephyr.h>
#include <init.h>

K_HEAP_DEFINE(event_mem_pool, CONFIG_EVENT_MEM_POOL_MAX_SIZE *
	      CONFIG_EVENT_MEM_POOL_NUMBER_BLOCKS);

void *app_alloc(size_t size)
{
	return k_heap_alloc(&event_mem_pool, size, K_NO_WAIT);
}

void app_free(void *ptr)
{
	k_heap_free(&event_mem_pool, ptr);
}