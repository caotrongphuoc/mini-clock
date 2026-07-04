#ifndef __TASK_MC_RTC_H__
#define __TASK_MC_RTC_H__

#include "rtc.h"

typedef struct {
	rtc_time_t time;
	rtc_date_t date;
	uint8_t    apply_time;
	uint8_t    apply_date;
} mc_clock_rtc_set_time_req_t;

typedef struct {
	rtc_time_t time;
	uint8_t    weekday;
} mc_clock_rtc_set_alarm_req_t;

typedef struct {
	uint32_t seconds;
} mc_clock_rtc_set_wakeup_req_t;

#endif /* __TASK_MC_RTC_H__ */
