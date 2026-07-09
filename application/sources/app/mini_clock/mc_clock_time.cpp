#include "mc_clock_time.h"

#include "buzzer.h"

/*****************************************************************************/
/* Variable Declaration - Clock object */
/*****************************************************************************/

static mc_clock_time_state_t clock_state;
static uint8_t clock_chime_enabled;

/*****************************************************************************/
/* Public API - Clock object */
/*****************************************************************************/

void mc_clock_time_get_state(mc_clock_time_state_t* state)
{
	*state = clock_state;
}

void mc_clock_time_set_chime_enabled(uint8_t enabled)
{
	clock_chime_enabled = enabled ? 1 : 0;
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

		if (clock_chime_enabled &&
		    clock_state.time.min == 0 &&
		    clock_state.time.sec == 0)
		{
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
	}
	break;

	default:
		break;
	}
}
