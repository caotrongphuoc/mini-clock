#ifndef __MC_CLOCK_STOPWATCH_H__
#define __MC_CLOCK_STOPWATCH_H__

#include "ak.h"

#include "app.h"
#include "message.h"
#include "task_list.h"

typedef struct
{
	uint8_t running;
	uint32_t elapsed_ms;
} mc_clock_stopwatch_state_t;

extern mc_clock_stopwatch_state_t mc_clock_stopwatch_state;

void mc_clock_stopwatch_handle(ak_msg_t* msg);
void mc_clock_stopwatch_get_state(mc_clock_stopwatch_state_t* state);

#endif //__MC_CLOCK_STOPWATCH_H__
