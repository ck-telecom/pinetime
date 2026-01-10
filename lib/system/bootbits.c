/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/drivers/retained_mem.h>
#include <zephyr/device.h>

#include "system/bootbits.h"

#include "system/logging.h"
#include "system/version.h"

#include <inttypes.h>
#include <stdint.h>
#include <string.h>

/* Define the retained memory device - adjust the alias based on your device tree */
const static struct device *retained_mem_device = DEVICE_DT_GET(DT_ALIAS(retainedmem));

/* Offset definitions for bootbits and version in retained memory */
#define BOOTBITS_OFFSET 0
#define VERSION_OFFSET 4

void boot_bit_init(void) {
  if (!device_is_ready(retained_mem_device)) {
    PBL_LOG(LOG_LEVEL_ERROR, "Retained memory device not ready!");
    return;
  }

  if (!boot_bit_test(BOOT_BIT_INITIALIZED)) {
    boot_bit_set(BOOT_BIT_INITIALIZED);
  }
}

void boot_bit_set(BootBitValue bit) {
  uint32_t current_value = boot_bits_get();
  current_value |= bit;

  int32_t rc = retained_mem_write(retained_mem_device, BOOTBITS_OFFSET, (void *)&current_value, sizeof(current_value));
  if (rc != 0) {
    PBL_LOG(LOG_LEVEL_ERROR, "Failed to set boot bit: %d", rc);
  }
}

void boot_bit_clear(BootBitValue bit) {
  uint32_t current_value = boot_bits_get();
  current_value &= ~bit;

  int32_t rc = retained_mem_write(retained_mem_device, BOOTBITS_OFFSET,
                                 &current_value, sizeof(current_value));
  if (rc != 0) {
    PBL_LOG(LOG_LEVEL_ERROR, "Failed to clear boot bit: %d", rc);
  }
}

bool boot_bit_test(BootBitValue bit) {
  return (boot_bits_get() & bit) != 0;
}

void boot_bit_dump(void) {
  PBL_LOG(LOG_LEVEL_DEBUG, "0x%"PRIx32, boot_bits_get());
}

uint32_t boot_bits_get(void) {
  uint32_t bootbits = 0;

  int32_t rc = retained_mem_read(retained_mem_device, BOOTBITS_OFFSET,
                                &bootbits, sizeof(bootbits));
  if (rc != 0) {
    PBL_LOG(LOG_LEVEL_ERROR, "Failed to read boot bits: %d", rc);
  }

  return bootbits;
}

void command_boot_bits_get(void) {
  char buffer[32];
  dbgserial_putstr_fmt(buffer, sizeof(buffer), "bootbits: 0x%"PRIu32, boot_bits_get());
}

void boot_version_write(void) {

}

uint32_t boot_version_read(void) {
  uint32_t version = 0;

  int32_t rc = retained_mem_read(retained_mem_device, VERSION_OFFSET,
                                &version, sizeof(version));
  if (rc != 0) {
    PBL_LOG(LOG_LEVEL_ERROR, "Failed to read boot version: %d", rc);
  }

  return version;
}
