#ifndef __MC_CLOCK_DATE_H__
#define __MC_CLOCK_DATE_H__

#include <stdint.h>

#include "rtc.h"

/* Gregorian date helpers shared by the clock objects, the screens and the shell. */

static inline uint8_t mc_clock_date_is_leap(uint16_t year)
{
	return ((year % 4 == 0) && (year % 100 != 0 || year % 400 == 0)) ? 1 : 0;
}

static inline uint8_t mc_clock_date_days_in_month(uint16_t year, uint8_t month)
{
	static const uint8_t days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (month < 1 || month > 12)
	{
		return 30;
	}

	if (month == 2 && mc_clock_date_is_leap(year))
	{
		return 29;
	}

	return days[month];
}

/* Day of the week, RTC_WEEKDAY_MON (1) .. RTC_WEEKDAY_SUN (7) (Sakamoto's method) */
static inline uint8_t mc_clock_date_weekday(uint16_t year, uint8_t month, uint8_t day)
{
	static const uint8_t offset[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

	if (month < 3)
	{
		year--;
	}

	uint8_t dow = (year + year / 4 - year / 100 + year / 400 + offset[month - 1] + day) % 7; /* 0 = Sunday */
	return (dow == 0) ? RTC_WEEKDAY_SUN : dow;
}

#endif //__MC_CLOCK_DATE_H__
