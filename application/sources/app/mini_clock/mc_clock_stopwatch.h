#ifndef __MC_CLOCK_STOPWATCH_H__
#define __MC_CLOCK_STOPWATCH_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

// Number of laps kept per run
#define STOPWATCH_LAP_NUMBER (10)

typedef struct
{
	uint8_t running;
	uint32_t elapsed_ms;
	uint8_t lap_count;
	uint32_t lap_ms[STOPWATCH_LAP_NUMBER];
} mc_clock_stopwatch_t;

extern mc_clock_stopwatch_t mc_clock_stopwatch;

#endif //__MC_CLOCK_STOPWATCH_H__
