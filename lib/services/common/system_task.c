#include "services/common/system_task.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>
#include <stdint.h>

#include "kernel/pebble_tasks.h"
#include "drivers/task_watchdog.h"

// System task constants
#define SYSTEM_TASK_STACK_SIZE 1024
#define SYSTEM_TASK_PRIORITY K_PRIO_PREEMPT(10)
#define SYSTEM_TASK_RAISED_PRIORITY K_PRIO_PREEMPT(5)
#define SYSTEM_TASK_QUEUE_SIZE 32

// System task structure
struct system_task_callback {
  SystemTaskEventCallback callback;
  void *data;
};

// System task thread
static struct k_thread s_system_task_thread;
static K_THREAD_STACK_DEFINE(s_system_task_stack, SYSTEM_TASK_STACK_SIZE);

// System task queue
static struct k_msgq s_system_task_queue;
static struct system_task_callback s_system_task_queue_buffer[SYSTEM_TASK_QUEUE_SIZE];

// System task synchronization
static struct k_mutex s_system_task_mutex;

// System task state
static bool s_system_task_blocked = false;
static bool s_system_task_raised_priority = false;
static SystemTaskEventCallback s_current_callback = NULL;

// System task function
static void system_task_entry(void *p1, void *p2, void *p3);

// Initialize system task
void system_task_init(void) {
  // Initialize queue
  k_msgq_init(&s_system_task_queue, (char *)(void *)s_system_task_queue_buffer,
             sizeof(struct system_task_callback), SYSTEM_TASK_QUEUE_SIZE);

  // Initialize mutex
  k_mutex_init(&s_system_task_mutex);

  // Create system task
  k_thread_create(&s_system_task_thread, s_system_task_stack,
                 K_THREAD_STACK_SIZEOF(s_system_task_stack),
                 system_task_entry, NULL, NULL, NULL,
                 SYSTEM_TASK_PRIORITY, 0, K_NO_WAIT);

  // Set thread name
  k_thread_name_set(&s_system_task_thread, "system_task");
}

// Initialize system task timer (if needed)
void system_task_timer_init(void) {
  // No timer initialization needed for Zephyr implementation
}

// Feed watchdog from system task
void system_task_watchdog_feed(void) {
  task_watchdog_bit_set(PebbleTask_KernelBackground);
}

// Add callback from ISR
bool system_task_add_callback_from_isr(SystemTaskEventCallback cb, void *data, bool* should_context_switch) {
  if (cb == NULL) {
    return false;
  }

  struct system_task_callback callback_data = {
    .callback = cb,
    .data = data
  };

  int ret = k_msgq_put(&s_system_task_queue, &callback_data, K_NO_WAIT);
  return (ret == 0);
}

// Add callback from thread
bool system_task_add_callback(SystemTaskEventCallback cb, void *data) {
  if (cb == NULL) {
    return false;
  }

  struct system_task_callback callback_data = {
    .callback = cb,
    .data = data
  };

  // Check if callbacks are blocked
  k_mutex_lock(&s_system_task_mutex, K_FOREVER);
  bool blocked = s_system_task_blocked;
  k_mutex_unlock(&s_system_task_mutex);

  if (blocked) {
    return false;
  }

  int ret = k_msgq_put(&s_system_task_queue, &callback_data, K_NO_WAIT);
  return (ret == 0);
}

// Block/unblock callbacks
void system_task_block_callbacks(bool block) {
  k_mutex_lock(&s_system_task_mutex, K_FOREVER);
  s_system_task_blocked = block;
  k_mutex_unlock(&s_system_task_mutex);
}

// Get available space in queue
uint32_t system_task_get_available_space(void) {
  return k_msgq_num_free_get(&s_system_task_queue);
}

// Get current callback
void* system_task_get_current_callback(void) {
  return (void*)s_current_callback;
}

// Enable raised priority
void system_task_enable_raised_priority(bool is_raised) {
  k_mutex_lock(&s_system_task_mutex, K_FOREVER);
  s_system_task_raised_priority = is_raised;

  // Set thread priority
  if (is_raised) {
    k_thread_priority_set(&s_system_task_thread, SYSTEM_TASK_RAISED_PRIORITY);
  } else {
    k_thread_priority_set(&s_system_task_thread, SYSTEM_TASK_PRIORITY);
  }

  k_mutex_unlock(&s_system_task_mutex);
}

// Check if system task is ready to run
bool system_task_is_ready_to_run(void) {
  // Check if queue has messages or if task is not blocked
  return (k_msgq_num_used_get(&s_system_task_queue) > 0) ||
         (!s_system_task_blocked && k_msgq_num_free_get(&s_system_task_queue) > 0);
}

// System task entry function
static void system_task_entry(void *p1, void *p2, void *p3) {
  struct system_task_callback callback_data;

  // Set task name for debugging
  k_thread_name_set(k_current_get(), "system_task");

  while (1) {
    // Take callback from queue (block indefinitely)
    if (k_msgq_get(&s_system_task_queue, &callback_data, K_FOREVER) == 0) {
      // Set current callback
      s_current_callback = callback_data.callback;

      // Feed watchdog
      system_task_watchdog_feed();

      // Execute callback
      callback_data.callback(callback_data.data);

      // Clear current callback
      s_current_callback = NULL;

      // Feed watchdog again after callback execution
      system_task_watchdog_feed();
    }
  }
}
