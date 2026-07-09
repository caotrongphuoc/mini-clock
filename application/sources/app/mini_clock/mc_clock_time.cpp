#include "mc_clock_time.h"

/*****************************************************************************/
/* Variable Declaration - Clock object */
/*****************************************************************************/

static mc_clock_time_state_t clock_state;

/*****************************************************************************/
/* Public API - Clock object */
/*****************************************************************************/

void mc_clock_time_get_state(mc_clock_time_state_t* state)
{
	*state = clock_state;
}

/*****************************************************************************/
/* Handle - Clock object */
/*****************************************************************************/

void mc_clock_time_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_TIME_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_TIME_SETUP\n");
		rtc_get_time(&clock_state.time);
		rtc_get_date(&clock_state.date);
	}
	break;

	case MC_CLOCK_TIME_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_TIME_UPDATE\n");
		rtc_get_time(&clock_state.time);
		rtc_get_date(&clock_state.date);
	}
	break;

	default:
		break;
	}
}
