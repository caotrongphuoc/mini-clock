#ifndef __MC_CLOCK_STOPWATCH_H__
#define __MC_CLOCK_STOPWATCH_H__

#include "ak.h"

#include "app.h"
#include "message.h"
#include "task_list.h"
#include "app_dbg.h"

#define MC_CLOCK_STOPWATCH_LAP_MAX (3)

typedef struct
{
	uint8_t running;
	uint32_t elapsed_ms;
	uint8_t lap_count;
	uint32_t lap_ms[MC_CLOCK_STOPWATCH_LAP_MAX];
} mc_clock_stopwatch_state_t;

void mc_clock_stopwatch_handle(ak_msg_t* msg);
void mc_clock_stopwatch_get_state(mc_clock_stopwatch_state_t* state);

#endif //__MC_CLOCK_STOPWATCH_H__
