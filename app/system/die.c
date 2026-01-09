/* SPDX-FileCopyrightText: 2024 Google LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "system/bootbits.h"
#include "system/die.h"
#include "system/reboot_reason.h"

NORETURN reset_due_to_software_failure(void) {

  boot_bit_set(BOOT_BIT_SOFTWARE_FAILURE_OCCURRED);
  system_reset();
}
