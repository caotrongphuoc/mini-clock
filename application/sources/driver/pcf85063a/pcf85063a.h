/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Jaakko Salo
 *
 * Portions adapted from https://github.com/jvsalo/pcf85063a for this project.
 */

#ifndef __PCF85063A_H__
#define __PCF85063A_H__

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>

#include "rtc.h"

void pcf85063a_init(void);

uint8_t pcf85063a_is_time_lost(void);

uint8_t pcf85063a_get_time(rtc_time_t* time);
uint8_t pcf85063a_set_time(const rtc_time_t* time);

uint8_t pcf85063a_get_date(rtc_date_t* date);
uint8_t pcf85063a_set_date(const rtc_date_t* date);

uint8_t pcf85063a_set_alarm(const rtc_time_t* time, uint8_t weekday);
uint8_t pcf85063a_clear_alarm(void);

uint8_t pcf85063a_set_countdown(uint32_t seconds);
uint8_t pcf85063a_stop_countdown(void);

#if defined(__cplusplus)
}
#endif

#endif //__PCF85063A_H__
