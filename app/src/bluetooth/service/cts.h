/** @file
 *  @brief CTS Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

void cts_init(void);
void cts_notify(void);

struct cts_current_time {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t dayofweek;
	uint8_t fractions;
	uint8_t adjust_reason;
};

#ifdef __cplusplus
}
#endif
