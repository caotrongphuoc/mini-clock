#ifndef __MC_CLOCK_TIME_H__
#define __MC_CLOCK_TIME_H__

#include "ak.h"
#include "rtc.h"

#include "app.h"
#include "message.h"
#include "task_list.h"
#include "app_dbg.h"

typedef struct
{
	rtc_time_t time;
	rtc_date_t date;
} mc_clock_time_state_t;

void mc_clock_time_handle(ak_msg_t* msg);
void mc_clock_time_get_state(mc_clock_time_state_t* state);
void mc_clock_time_set_chime_enabled(uint8_t enabled);
void mc_clock_time_adjust_timezone(const rtc_time_t* in_time, const rtc_date_t* in_date, int16_t offset_minutes, rtc_time_t* out_time, rtc_date_t* out_date);
void mc_clock_time_update_local(void);

#endif //__MC_CLOCK_TIME_H__
