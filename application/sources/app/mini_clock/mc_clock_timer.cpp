#include "mc_clock_timer.h"

#include "mc_clock_alarm.h"

static mc_clock_timer_state_t mc_clock_timer_state = {
    .set_hour = 0,
    .set_min = 1,
    .set_sec = 0,

    .remaining_hour = 0,
    .remaining_min = 1,
    .remaining_sec = 0,

    .running = 0,
    .finished = 0,

    .edit_field = MC_TIMER_EDIT_MIN,
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

			mc_clock_timer_state.remaining_hour = mc_clock_timer_state.set_hour;
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_min;
			mc_clock_timer_state.remaining_sec = mc_clock_timer_state.set_sec;

			BUZZER_Disable();
			break;
		}

		if (!mc_clock_timer_state.running &&
		    mc_clock_timer_state.remaining_hour == 0 &&
		    mc_clock_timer_state.remaining_min == 0 &&
		    mc_clock_timer_state.remaining_sec == 0)
		{
			mc_clock_timer_state.remaining_hour = mc_clock_timer_state.set_hour;
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_min;
			mc_clock_timer_state.remaining_sec = mc_clock_timer_state.set_sec;
		}

		mc_clock_timer_state.running = !mc_clock_timer_state.running;
	}
	break;

	case MC_CLOCK_TIMER_RESET:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_RESET\n");
		mc_clock_timer_state.running = 0;
		mc_clock_timer_state.finished = 0;

		mc_clock_timer_state.remaining_hour = mc_clock_timer_state.set_hour;
		mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_min;
		mc_clock_timer_state.remaining_sec = mc_clock_timer_state.set_sec;

		BUZZER_Disable();
	}
	break;

	case MC_CLOCK_TIMER_INC:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_INC\n");
		if (!mc_clock_timer_state.running)
		{
			mc_clock_timer_state.finished = 0;

			switch (mc_clock_timer_state.edit_field)
			{
			case MC_TIMER_EDIT_HOUR:
				if (mc_clock_timer_state.set_hour < 23)
					mc_clock_timer_state.set_hour++;
				break;

			case MC_TIMER_EDIT_MIN:
				if (mc_clock_timer_state.set_min < 59)
					mc_clock_timer_state.set_min++;
				break;

			case MC_TIMER_EDIT_SEC:
				if (mc_clock_timer_state.set_sec < 59)
					mc_clock_timer_state.set_sec++;
				break;
			}

			mc_clock_timer_state.remaining_hour = mc_clock_timer_state.set_hour;
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_min;
			mc_clock_timer_state.remaining_sec = mc_clock_timer_state.set_sec;
		}
	}
	break;

	case MC_CLOCK_TIMER_DEC:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_DEC\n");
		if (!mc_clock_timer_state.running)
		{
			mc_clock_timer_state.finished = 0;

			switch (mc_clock_timer_state.edit_field)
			{
			case MC_TIMER_EDIT_HOUR:
				if (mc_clock_timer_state.set_hour > 0)
					mc_clock_timer_state.set_hour--;
				break;

			case MC_TIMER_EDIT_MIN:
				if (mc_clock_timer_state.set_min > 0)
					mc_clock_timer_state.set_min--;
				break;

			case MC_TIMER_EDIT_SEC:
				if (mc_clock_timer_state.set_sec > 0)
					mc_clock_timer_state.set_sec--;
				break;
			}

			mc_clock_timer_state.remaining_hour = mc_clock_timer_state.set_hour;
			mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_min;
			mc_clock_timer_state.remaining_sec = mc_clock_timer_state.set_sec;
		}
	}
	break;

	case MC_CLOCK_TIMER_NEXT_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_NEXT_FIELD\n");
		if (!mc_clock_timer_state.running)
		{
			mc_clock_timer_state.edit_field++;

			if (mc_clock_timer_state.edit_field > MC_TIMER_EDIT_SEC)
			{
				mc_clock_timer_state.edit_field = MC_TIMER_EDIT_HOUR;
			}
		}
	}
	break;

	case MC_CLOCK_TIMER_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_UPDATE\n");
		if (!mc_clock_timer_state.running)
			break;

		if (mc_clock_timer_state.remaining_sec > 0)
		{
			mc_clock_timer_state.remaining_sec--;
		}
		else if (mc_clock_timer_state.remaining_min > 0)
		{
			mc_clock_timer_state.remaining_min--;
			mc_clock_timer_state.remaining_sec = 59;
		}
		else if (mc_clock_timer_state.remaining_hour > 0)
		{
			mc_clock_timer_state.remaining_hour--;
			mc_clock_timer_state.remaining_min = 59;
			mc_clock_timer_state.remaining_sec = 59;
		}
		else
		{
			mc_clock_timer_state.running = 0;
			mc_clock_timer_state.finished = 1;
			BUZZER_PlaySound((buzzer_sound_t)mc_clock_alarm_get_sound());
		}
	}
	break;

	case MC_CLOCK_TIMER_DISMISS:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_DISMISS\n");
		mc_clock_timer_state.running = 0;
		mc_clock_timer_state.finished = 0;
		mc_clock_timer_state.remaining_hour = mc_clock_timer_state.set_hour;
		mc_clock_timer_state.remaining_min = mc_clock_timer_state.set_min;
		mc_clock_timer_state.remaining_sec = mc_clock_timer_state.set_sec;
		BUZZER_Disable();
	}
	break;

	default:
		break;
	}
}
