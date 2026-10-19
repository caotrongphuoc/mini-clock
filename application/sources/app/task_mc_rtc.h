#ifndef __TASK_MC_RTC_H__
#define __TASK_MC_RTC_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "rtc.h"

#include "mc_clock_date.h"

// MC_CLOCK_RTC_SET_TIME_REQ payload
typedef struct
{
	rtc_time_t time;
	rtc_date_t date;
	uint8_t apply_time;
	uint8_t apply_date;
} mc_clock_rtc_set_time_req_t;

// MC_CLOCK_RTC_SET_ALARM_REQ payload
typedef struct
{
	rtc_time_t time;
	uint8_t weekday;
} mc_clock_rtc_set_alarm_req_t;

#endif //__TASK_MC_RTC_H__
