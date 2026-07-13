#include "mc_clock_timer.h"

static mc_clock_timer_state_t mc_clock_timer_state = {
    .set_minutes = 5,
    .remaining_min = 5,
    .remaining_sec = 0,
    .running = 0,
    .finished = 0,
};

void mc_clock_timer_get_state(mc_clock_timer_state_t* state)
{
	*state = mc_clock_timer_state;
}

void mc_clock_timer_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_TIMER_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_SETUP\n");
	}
	break;

	case MC_CLOCK_TIMER_START_PAUSE:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_START_PAUSE\n");
		if (mc_clock_timer_state.finished)
		{
			mc_clock_timer_state.finished = 0;
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_minutes;
			mc_clock_timer_state.remaining_sec = 0;
			BUZZER_Disable();
			break;
		}

		if (mc_clock_timer_state.running == 0 &&
		    mc_clock_timer_state.remaining_min == 0 &&
		    mc_clock_timer_state.remaining_sec == 0)
		{
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_minutes;
		}

		mc_clock_timer_state.running = !mc_clock_timer_state.running;
	}
	break;

	case MC_CLOCK_TIMER_RESET:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_RESET\n");
		mc_clock_timer_state.running = 0;
		mc_clock_timer_state.finished = 0;
		mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_minutes;
		mc_clock_timer_state.remaining_sec = 0;
		BUZZER_Disable();
	}
	break;

	case MC_CLOCK_TIMER_INC:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_INC\n");
		if (mc_clock_timer_state.running == 0)
		{
			mc_clock_timer_state.finished = 0;
			mc_clock_timer_state.set_minutes += MC_CLOCK_TIMER_STEP_MINUTES;
			if (mc_clock_timer_state.set_minutes > MC_CLOCK_TIMER_MAX_MINUTES)
			{
				mc_clock_timer_state.set_minutes = MC_CLOCK_TIMER_MAX_MINUTES;
			}
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_minutes;
			mc_clock_timer_state.remaining_sec = 0;
		}
	}
	break;

	case MC_CLOCK_TIMER_DEC:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_DEC\n");
		if (mc_clock_timer_state.running == 0)
		{
			mc_clock_timer_state.finished = 0;
			if (mc_clock_timer_state.set_minutes >= MC_CLOCK_TIMER_STEP_MINUTES)
			{
				mc_clock_timer_state.set_minutes -= MC_CLOCK_TIMER_STEP_MINUTES;
			}
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_minutes;
			mc_clock_timer_state.remaining_sec = 0;
		}
	}
	break;

	case MC_CLOCK_TIMER_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_UPDATE\n");
		if (mc_clock_timer_state.running == 0)
		{
			break;
		}

		if (mc_clock_timer_state.remaining_sec > 0)
		{
			mc_clock_timer_state.remaining_sec--;
			break;
		}

		if (mc_clock_timer_state.remaining_min > 0)
		{
			mc_clock_timer_state.remaining_min--;
			mc_clock_timer_state.remaining_sec = 59;
			break;
		}

		mc_clock_timer_state.running = 0;
		mc_clock_timer_state.finished = 1;
		BUZZER_PlaySound(BUZZER_SOUND_GOODBYE);
	}
	break;

	case MC_CLOCK_TIMER_DISMISS:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_DISMISS\n");
		mc_clock_timer_state.running = 0;
		mc_clock_timer_state.finished = 0;
		mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_minutes;
		mc_clock_timer_state.remaining_sec = 0;
		BUZZER_Disable();
	}
	break;

	default:
		break;
	}
}
