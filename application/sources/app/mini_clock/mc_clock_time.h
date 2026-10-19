#ifndef __MC_CLOCK_TIME_H__
#define __MC_CLOCK_TIME_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "rtc.h"

#include "mc_clock_world.h"
#include "mc_clock_date.h"

typedef struct
{
	rtc_time_t time;
	rtc_date_t date;
} mc_clock_time_t;

extern mc_clock_time_t mc_clock_time;

extern void mc_clock_time_update_local(void);
extern void mc_clock_time_set_chime_enabled(uint8_t enabled);
extern void mc_clock_time_adjust_timezone(const rtc_time_t* in_time, const rtc_date_t* in_date, int16_t offset_minutes, rtc_time_t* out_time, rtc_date_t* out_date);

#endif //__MC_CLOCK_TIME_H__
