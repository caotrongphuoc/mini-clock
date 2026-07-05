#ifndef __MC_CLOCK_CLOCK_H__
#define __MC_CLOCK_CLOCK_H__

#include "ak.h"
#include "rtc.h"

#include "app.h"
#include "message.h"
#include "task_list.h"

typedef struct
{
	rtc_time_t time;
	rtc_date_t date;
	uint8_t format_24h;
} mc_clock_clock_state_t;

void mc_clock_clock_handle(ak_msg_t* msg);

void mc_clock_clock_sync();
void mc_clock_clock_set_24h_format(uint8_t format_24h);
void mc_clock_clock_toggle_format();
void mc_clock_clock_get_state(mc_clock_clock_state_t* state);

#endif //__MC_CLOCK_CLOCK_H__
