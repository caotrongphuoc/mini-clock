#ifndef __MC_CLOCK_STOPWATCH_H__
#define __MC_CLOCK_STOPWATCH_H__

#include "ak.h"

#include "app.h"
#include "message.h"
#include "timer.h"
#include "task_list.h"
#include "app_dbg.h"

// Number of laps kept per run
#define STOPWATCH_LAP_NUMBER (10)

typedef struct
{
	uint8_t running;
	uint32_t elapsed_ms;
	uint8_t lap_count;
	uint32_t lap_ms[STOPWATCH_LAP_NUMBER];
} mc_clock_stopwatch_t;

void mc_clock_stopwatch_handle(ak_msg_t* msg);
extern mc_clock_stopwatch_t mc_clock_stopwatch;

#endif //__MC_CLOCK_STOPWATCH_H__
