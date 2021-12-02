#ifndef _EVENT_SERVICE_MEM_H
#define _EVENT_SERVICE_MEM_H

#include <stddef.h>

#ifndef CONFIG_EVENT_MEM_POOL_MAX_SIZE
#define CONFIG_EVENT_MEM_POOL_MAX_SIZE 1024
#endif

#ifndef CONFIG_EVENT_MEM_POOL_NUMBER_BLOCKS
#define CONFIG_EVENT_MEM_POOL_NUMBER_BLOCKS 1
#endif

void *app_alloc(size_t size);

void app_free(void *ptr);

#endif /* _EVENT_SERVICE_MEM_H */