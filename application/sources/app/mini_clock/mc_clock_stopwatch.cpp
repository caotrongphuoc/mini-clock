#include "mc_clock_stopwatch.h"

/*****************************************************************************/
/* Variable Declaration - Clock stopwatch object */
/*****************************************************************************/

static mc_clock_stopwatch_state_t mc_clock_stopwatch_state;

/*****************************************************************************/
/* Public API - Clock stopwatch object */
/*****************************************************************************/

void mc_clock_stopwatch_get_state(mc_clock_stopwatch_state_t* state)
{
	*state = mc_clock_stopwatch_state;
}

/*****************************************************************************/
/* Handle - Clock stopwatch object */
/*****************************************************************************/

void mc_clock_stopwatch_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_STOPWATCH_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_STOPWATCH_SETUP\n");
	}
	break;

	case MC_CLOCK_STOPWATCH_START_PAUSE:
	{
		APP_DBG_SIG("MC_CLOCK_STOPWATCH_START_PAUSE\n");
		mc_clock_stopwatch_state.running = !mc_clock_stopwatch_state.running;
	}
	break;

	case MC_CLOCK_STOPWATCH_RESET:
	{
		APP_DBG_SIG("MC_CLOCK_STOPWATCH_RESET\n");
		mc_clock_stopwatch_state.running = 0;
		mc_clock_stopwatch_state.elapsed_ms = 0;
		mc_clock_stopwatch_state.lap_count = 0;
	}
	break;

	case MC_CLOCK_STOPWATCH_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_STOPWATCH_UPDATE\n");
		if (mc_clock_stopwatch_state.running)
		{
			mc_clock_stopwatch_state.elapsed_ms += MC_CLOCK_STOPWATCH_TICK_INTERVAL;
		}
	}
	break;

	case MC_CLOCK_STOPWATCH_LAP:
	{
		APP_DBG_SIG("MC_CLOCK_STOPWATCH_LAP\n");
		if (mc_clock_stopwatch_state.running &&
		    mc_clock_stopwatch_state.lap_count < MC_CLOCK_STOPWATCH_LAP_MAX)
		{
			mc_clock_stopwatch_state.lap_ms[mc_clock_stopwatch_state.lap_count] =
			    mc_clock_stopwatch_state.elapsed_ms;
			mc_clock_stopwatch_state.lap_count++;
		}
	}
	break;

	case MC_CLOCK_STOPWATCH_CLEAR_LAP:
	{
		APP_DBG_SIG("MC_CLOCK_STOPWATCH_CLEAR_LAP\n");
		if (mc_clock_stopwatch_state.running == 0)
		{
			mc_clock_stopwatch_state.lap_count = 0;
		}
	}
	break;

	default:
		break;
	}
}
