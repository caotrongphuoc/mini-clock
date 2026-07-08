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

extern mc_clock_time_state_t clock_state;

void mc_clock_time_handle(ak_msg_t* msg);
void mc_clock_time_get_state(mc_clock_time_state_t* state);

#endif //__MC_CLOCK_TIME_H__
