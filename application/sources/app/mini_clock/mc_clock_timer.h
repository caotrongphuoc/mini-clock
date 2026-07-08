#ifndef __MC_CLOCK_TIMER_H__
#define __MC_CLOCK_TIMER_H__

#include "ak.h"

#include "app.h"
#include "message.h"
#include "task_list.h"

#define MC_CLOCK_TIMER_MAX_MINUTES (1000)
#define MC_CLOCK_TIMER_STEP_MINUTES (5)

typedef struct
{
	uint16_t set_minutes;
	uint16_t remaining_min;
	uint8_t remaining_sec;
	uint8_t running;
	uint8_t finished;
} mc_clock_timer_state_t;

extern mc_clock_timer_state_t mc_clock_timer_state;

void mc_clock_timer_handle(ak_msg_t* msg);
void mc_clock_timer_get_state(mc_clock_timer_state_t* state);

#endif //__MC_CLOCK_TIMER_H__
