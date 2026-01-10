#pragma once

#include <zephyr/logging/log.h>
#include <stdint.h>

// Status code macros
#define PASSED(s) ((int32_t)(s) >= 0)
#define FAILED(s) ((int32_t)(s) < 0)

#ifndef __FILE_NAME__
#ifdef __FILE_NAME_LEGACY__
#define __FILE_NAME__ __FILE_NAME_LEGACY__
#else
#define __FILE_NAME__ __FILE__
#endif
#endif

// PebbleOS log levels mapped to Zephyr log levels
#define LOG_LEVEL_ALWAYS    0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARNING   50
#define LOG_LEVEL_INFO      100
#define LOG_LEVEL_DEBUG     200
#define LOG_LEVEL_DEBUG_VERBOSE 255

// Zephyr log level mapping
#define _PBL_LOG_LEVEL_MAP(level) \
  ((level) <= LOG_LEVEL_ERROR) ? LOG_LEVEL_ERR : \
  ((level) <= LOG_LEVEL_WARNING) ? LOG_LEVEL_WRN : \
  ((level) <= LOG_LEVEL_INFO) ? LOG_LEVEL_INF : \
  ((level) <= LOG_LEVEL_DEBUG) ? LOG_LEVEL_DBG : LOG_LEVEL_DBG

// Default log level
#ifndef PBL_LOG_LEVEL
#define PBL_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

// Log domain definitions
#define LOG_DOMAIN_BT                 1
#define LOG_DOMAIN_MISC               1
#define LOG_DOMAIN_FS                 1
#define LOG_DOMAIN_COMM               1
#define LOG_DOMAIN_ACCEL              0
#define LOG_DOMAIN_TEXT               0
#define LOG_DOMAIN_QEMU_COMM          0
#define LOG_DOMAIN_ANIMATION          0
#define LOG_DOMAIN_ANALYTICS          0
#define LOG_DOMAIN_ACTIVITY           0
#define LOG_DOMAIN_ACTIVITY_INSIGHTS  0
#define LOG_DOMAIN_PROTOBUF           0
#define LOG_DOMAIN_VOICE              0
#define LOG_DOMAIN_BLOBDB             0
#define LOG_DOMAIN_BT_PAIRING_INFO    0
#define LOG_DOMAIN_BT_STACK           0
#define LOG_DOMAIN_DATA_LOGGING       0
#define LOG_DOMAIN_TOUCH              0
#define LOG_DOMAIN_I2C                0

// Default log domain
#ifndef DEFAULT_LOG_DOMAIN
#define DEFAULT_LOG_DOMAIN LOG_DOMAIN_MISC
#endif

// Default log level for the module
#ifndef DEFAULT_LOG_LEVEL
#define DEFAULT_LOG_LEVEL PBL_LOG_LEVEL
#endif

// Check if logging should be enabled for a given level
#define PBL_SHOULD_LOG(level) ((level) <= DEFAULT_LOG_LEVEL)

// Define log module for Zephyr
LOG_MODULE_REGISTER(pbl_log, LOG_LEVEL_DBG);

// Main logging macro implementation
#define PBL_LOG(level, fmt, ...) \
  do { \
    if (PBL_SHOULD_LOG(level)) { \
      switch (_PBL_LOG_LEVEL_MAP(level)) { \
        case LOG_LEVEL_ERR: \
          LOG_ERR(fmt, ## __VA_ARGS__); \
          break; \
        case LOG_LEVEL_WRN: \
          LOG_WRN(fmt, ## __VA_ARGS__); \
          break; \
        case LOG_LEVEL_INF: \
          LOG_INF(fmt, ## __VA_ARGS__); \
          break; \
        case LOG_LEVEL_DBG: \
          LOG_DBG(fmt, ## __VA_ARGS__); \
          break; \
        default: \
          LOG_INF(fmt, ## __VA_ARGS__); \
          break; \
      } \
    } \
  } while (0)

// Domain-specific logging
#define PBL_LOG_D(domain, level, fmt, ...) \
  do { \
    if ((domain) && PBL_SHOULD_LOG(level)) { \
      switch (_PBL_LOG_LEVEL_MAP(level)) { \
        case LOG_LEVEL_ERR: \
          LOG_ERR(fmt, ## __VA_ARGS__); \
          break; \
        case LOG_LEVEL_WRN: \
          LOG_WRN(fmt, ## __VA_ARGS__); \
          break; \
        case LOG_LEVEL_INF: \
          LOG_INF(fmt, ## __VA_ARGS__); \
          break; \
        case LOG_LEVEL_DBG: \
          LOG_DBG(fmt, ## __VA_ARGS__); \
          break; \
        default: \
          LOG_INF(fmt, ## __VA_ARGS__); \
          break; \
      } \
    } \
  } while (0)

// Synchronous logging (Zephyr doesn't have async/sync distinction)
#define PBL_LOG_SYNC(level, fmt, ...) PBL_LOG(level, fmt, ## __VA_ARGS__)
#define PBL_LOG_D_SYNC(domain, level, fmt, ...) PBL_LOG_D(domain, level, fmt, ## __VA_ARGS__)

// Color logging (Zephyr handles colors automatically)
#define PBL_LOG_COLOR(level, color, fmt, ...) PBL_LOG(level, fmt, ## __VA_ARGS__)
#define PBL_LOG_COLOR_D(domain, level, color, fmt, ...) PBL_LOG_D(domain, level, fmt, ## __VA_ARGS__)
#define PBL_LOG_COLOR_SYNC(level, color, fmt, ...) PBL_LOG_SYNC(level, fmt, ## __VA_ARGS__)
#define PBL_LOG_COLOR_D_SYNC(domain, level, color, fmt, ...) PBL_LOG_D_SYNC(domain, level, fmt, ## __VA_ARGS__)

// Verbose logging
#ifdef VERBOSE_LOGGING
#define PBL_LOG_VERBOSE(fmt, ...) PBL_LOG(LOG_LEVEL_DEBUG, fmt, ## __VA_ARGS__)
#define PBL_LOG_D_VERBOSE(domain, fmt, ...) PBL_LOG_D(domain, LOG_LEVEL_DEBUG, fmt, ## __VA_ARGS__)
#define PBL_LOG_COLOR_VERBOSE(color, fmt, ...) PBL_LOG_COLOR(LOG_LEVEL_DEBUG, color, fmt, ## __VA_ARGS__)
#define PBL_LOG_COLOR_D_VERBOSE(domain, color, fmt, ...) PBL_LOG_COLOR_D(domain, LOG_LEVEL_DEBUG, color, fmt, ## __VA_ARGS__)
#else
#define PBL_LOG_VERBOSE(fmt, ...)
#define PBL_LOG_D_VERBOSE(domain, fmt, ...)
#define PBL_LOG_COLOR_VERBOSE(color, fmt, ...)
#define PBL_LOG_COLOR_D_VERBOSE(domain, color, fmt, ...)
#endif

// Status return macros
#define RETURN_STATUS_D(d, st) \
  do { \
    if ((st) != 0) { \
      PBL_LOG_D(d, LOG_LEVEL_WARNING, "Status: %d", (int)(st)); \
    } \
    return (st); \
  } while (0)

#define RETURN_STATUS_COLOR_D(d, color, st) \
  do { \
    if ((st) != 0) { \
      PBL_LOG_COLOR_D(d, LOG_LEVEL_WARNING, color, "Status: %d", (int)(st)); \
    } \
    return (st); \
  } while (0)

#define RETURN_STATUS_UP_D(d, st) \
  do { \
    if ((st) == E_INVALID_ARGUMENT) { \
      PBL_LOG_D(d, LOG_LEVEL_ERROR, "Status: %d", (int)(st)); \
      return E_INTERNAL; \
    } \
    return (st); \
  } while (0)

#define RETURN_STATUS_UP_COLOR_D(d, color, st) \
  do { \
    if ((st) == E_INVALID_ARGUMENT) { \
      PBL_LOG_COLOR_D(d, LOG_LEVEL_ERROR, color, "Status: %d", (int)(st)); \
      return E_INTERNAL; \
    } \
    return (st); \
  } while (0)

// Global macros
#define RETURN_STATUS(s) RETURN_STATUS_D(DEFAULT_LOG_DOMAIN, s)
#define RETURN_STATUS_UP(s) RETURN_STATUS_UP_D(DEFAULT_LOG_DOMAIN, s)

// Convenience logging macros for common log levels
#define PBL_LOGA(fmt, ...) PBL_LOG(LOG_LEVEL_ALWAYS, fmt, ## __VA_ARGS__)
#define PBL_LOGE(fmt, ...) PBL_LOG(LOG_LEVEL_ERROR, fmt, ## __VA_ARGS__)
#define PBL_LOGW(fmt, ...) PBL_LOG(LOG_LEVEL_WARNING, fmt, ## __VA_ARGS__)
#define PBL_LOGI(fmt, ...) PBL_LOG(LOG_LEVEL_INFO, fmt, ## __VA_ARGS__)
#define PBL_LOGD(fmt, ...) PBL_LOG(LOG_LEVEL_DEBUG, fmt, ## __VA_ARGS__)
