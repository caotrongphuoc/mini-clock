#ifndef __SCR_CLOCK_TIME_SETTING_H__
#define __SCR_CLOCK_TIME_SETTING_H__

#include "screens.h"
#include "mc_clock_time.h"
#include "scr_clock_main.h"

// Year range supported by the RTC
#define TIME_SETTING_YEAR_MIN (2000)
#define TIME_SETTING_YEAR_MAX (2099)

// Character size of the time row (text size 2)
#define TIME_SETTING_TIME_CHAR_W (12)
#define TIME_SETTING_TIME_CHAR_H (16)

// Character size of the date row (text size 1)
#define TIME_SETTING_INFO_CHAR_W (6)
#define TIME_SETTING_INFO_CHAR_H (8)

// Save button position
#define TIME_SETTING_SAVE_X (110)

typedef enum
{
	TIME_SETTING_YEAR,
	TIME_SETTING_MONTH,
	TIME_SETTING_DATE,
	TIME_SETTING_WEEKDAY,
	TIME_SETTING_HOUR,
	TIME_SETTING_MIN,
	TIME_SETTING_SEC,
	TIME_SETTING_SAVE,
	TIME_SETTING_FIELD_NUMBER,
} scr_clock_time_setting_field_t;

#endif //__SCR_CLOCK_TIME_SETTING_H__
