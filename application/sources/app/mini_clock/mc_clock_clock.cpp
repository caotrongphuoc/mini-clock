#include "mc_clock_clock.h"

/*****************************************************************************/
/* Variable Declaration - Clock object */
/*****************************************************************************/

static mc_clock_clock_state_t clock_state;

/*****************************************************************************/
/* Public API - Clock object */
/*****************************************************************************/

void mc_clock_clock_get_state(mc_clock_clock_state_t* state)
{
	*state = clock_state;
}

void mc_clock_clock_sync()
{
	rtc_get_time(&clock_state.time);
	rtc_get_date(&clock_state.date);
}

void mc_clock_clock_set_24h_format(uint8_t format_24h)
{
	clock_state.format_24h = format_24h;
}

void mc_clock_clock_toggle_format()
{
	clock_state.format_24h = !clock_state.format_24h;
}

/*****************************************************************************/
/* Handle - Clock object */
/*****************************************************************************/

void mc_clock_clock_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_CLOCK_ENTER:
	{
		mc_clock_clock_set_24h_format(1);
		mc_clock_clock_sync();
	}
	break;

	case MC_CLOCK_TIME_TICK:
	{
		mc_clock_clock_sync();
	}
	break;

	case MC_CLOCK_CLOCK_FORMAT_TOGGLE:
	{
		mc_clock_clock_toggle_format();
	}
	break;

	case MC_CLOCK_CLOCK_LEAVE:
	default:
		break;
	}
}
