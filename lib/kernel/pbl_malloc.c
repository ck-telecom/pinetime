/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "pbl_malloc.h"

#include <zephyr/kernel.h>
#include <string.h>

// Simplified Heap struct for Zephyr compatibility
typedef struct Heap {
  const char *name;
  // In Zephyr, we'll just use the default heap for all allocations
  // For more advanced implementation, we could use Zephyr's memory pools
} Heap;

// Global heap instances
static Heap g_kernel_heap = { .name = "kernel" };
static Heap g_app_heap = { .name = "app" };
static Heap g_worker_heap = { .name = "worker" };

Heap *task_heap_get_for_current_task(void) {
  // In this simplified implementation, we always return the kernel heap
  // For a more complete implementation, we would check the current task
  // and return the appropriate heap
  return &g_kernel_heap;
}

static char* prv_strdup(Heap *heap, const char* s, uintptr_t lr) {
  (void)heap; // Unused in this implementation
  (void)lr; // Unused in this implementation
  
  size_t len = strlen(s) + 1;
  char *dup = k_malloc(len);
  if (dup) {
    strcpy(dup, s);
  }
  return dup;
}

// task_* functions that map to other heaps depending on the current task
///////////////////////////////////////////////////////////
void *task_malloc(size_t bytes) {
  return k_malloc(bytes);
}

void *task_malloc_check(size_t bytes) {
  void *mem = k_malloc(bytes);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
    // In Zephyr, we can't easily "croak" like in PebbleOS
    // Instead, we'll just return NULL and let the caller handle it
  }
  return mem;
}

void task_free(void* ptr) {
  k_free(ptr);
}

void *task_realloc(void* ptr, size_t size) {
  return k_realloc(ptr, size);
}

void *task_zalloc(size_t size) {
  return k_zalloc(size);
}

void *task_zalloc_check(size_t bytes) {
  void *mem = k_zalloc(bytes);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

void *task_calloc(size_t count, size_t size) {
  return k_calloc(count, size);
}

void *task_calloc_check(size_t count, size_t size) {
  const size_t bytes = count * size;
  void *mem = k_calloc(count, size);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

char *task_strdup(const char *s) {
  return prv_strdup(NULL, s, 0);
}

// app_* functions that allocate on the app heap
///////////////////////////////////////////////////////////
void *app_malloc(size_t bytes) {
  return k_malloc(bytes);
}

void *app_malloc_check(size_t bytes) {
  void *mem = k_malloc(bytes);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

void app_free(void *ptr) {
  k_free(ptr);
}

void *app_realloc(void *ptr, size_t bytes) {
  return k_realloc(ptr, bytes);
}

void *app_zalloc(size_t size) {
  return k_zalloc(size);
}

void *app_zalloc_check(size_t bytes) {
  void *mem = k_zalloc(bytes);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

void *app_calloc(size_t count, size_t size) {
  return k_calloc(count, size);
}

void *app_calloc_check(size_t count, size_t size) {
  const size_t bytes = count * size;
  void *mem = k_calloc(count, size);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

char *app_strdup(const char *s) {
  return prv_strdup(NULL, s, 0);
}

// kernel_* functions that allocate on the kernel heap
///////////////////////////////////////////////////////////
void *kernel_malloc(size_t bytes) {
  return k_malloc(bytes);
}

void *kernel_malloc_check(size_t bytes) {
  void *mem = k_malloc(bytes);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

void *kernel_calloc(size_t count, size_t size) {
  return k_calloc(count, size);
}

void *kernel_calloc_check(size_t count, size_t size) {
  const size_t bytes = count * size;
  void *mem = k_calloc(count, size);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

void *kernel_realloc(void *ptr, size_t bytes) {
  return k_realloc(ptr, bytes);
}

void *kernel_zalloc(size_t size) {
  return k_zalloc(size);
}

void *kernel_zalloc_check(size_t bytes) {
  void *mem = k_zalloc(bytes);
  if (!mem && bytes != 0) {
    printk("Out of memory: requested %zu bytes\n", bytes);
  }
  return mem;
}

void kernel_free(void *ptr) {
  k_free(ptr);
}

char *kernel_strdup(const char *s) {
  return prv_strdup(NULL, s, 0);
}

char *kernel_strdup_check(const char *s) {
  char *mem = prv_strdup(NULL, s, 0);
  if (!mem) {
    printk("Out of memory: requested %zu bytes\n", strlen(s) + 1);
  }
  return mem;
}
