/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#pragma once

#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#define UNLIKELY(expr) __builtin_expect((expr), 0)

#define NORETURN __attribute__((noreturn))

#ifdef PBL_LOGS_HASHED
  #define PBL_ASSERT(expr, msg, ...) \
    do { \
      if (UNLIKELY(!(expr))) { \
        LOG_ERR("*** ASSERTION FAILED: " msg, ## __VA_ARGS__); \
        k_panic(); \
      } \
    } while (0)

  #define PBL_ASSERTN(expr) \
    do { \
      if (UNLIKELY(!(expr))) { \
        LOG_ERR("*** ASSERTION FAILED"); \
        k_panic(); \
      } \
    } while (0)

  #define PBL_ASSERTN_LR(expr, lr) PBL_ASSERTN(expr)

  NORETURN passert_failed_hashed(uint32_t packed_loghash, ...);
  NORETURN passert_failed_hashed_with_lr(uint32_t lr, uint32_t packed_loghash, ...);
  NORETURN passert_failed_hashed_no_message(void);
  NORETURN passert_failed_hashed_no_message_with_lr(uint32_t lr);

#else
  #define PBL_ASSERT(expr, ...) __ASSERT(expr, ## __VA_ARGS__)

  #define PBL_ASSERTN(expr) __ASSERT_NO_MSG(expr)

  #define PBL_ASSERTN_LR(expr, lr) PBL_ASSERTN(expr)

#endif

#define WTF wtf()

#if UNITTEST

#define PBL_ASSERT_TASK(task)
#define PBL_ASSERT_NOT_TASK(task)
#define PBL_ASSERT_RUNNING_FROM_EXPECTED_TASK(task)
#define BREAKPOINT

#else

#define BREAKPOINT __asm("bkpt")

enum PebbleTask;

void passert_check_task(enum PebbleTask expected_task);
void passert_check_not_task(enum PebbleTask unexpected_task);

#define PBL_ASSERT_TASK(task) passert_check_task(task)
#define PBL_ASSERT_NOT_TASK(task) passert_check_not_task(task)

#ifdef CHECK_RUNNING_FROM_EXPECTED_TASK
#define PBL_ASSERT_RUNNING_FROM_EXPECTED_TASK(task) PBL_ASSERT_TASK(task)
#else
#define PBL_ASSERT_RUNNING_FROM_EXPECTED_TASK(task)
#endif

#endif  // UNITTEST

#ifdef PBL_LOGS_HASHED
  #define PBL_CROAK(msg, ...) \
    do { \
      LOG_ERR("*** CROAK: " msg, ## __VA_ARGS__); \
      k_panic(); \
    } while (0)
#else
  #define PBL_CROAK(fmt, args...) \
    do { \
      LOG_ERR("*** CROAK: " fmt, ## args); \
      k_panic(); \
    } while (0)
#endif

typedef struct Heap Heap;

#define PBL_CROAK_OOM(bytes, saved_lr, heap_ptr) \
  do { \
    LOG_ERR("*** OUT OF MEMORY: requested %zu bytes", bytes); \
    croak_oom(bytes, saved_lr, heap_ptr); \
  } while (0)
