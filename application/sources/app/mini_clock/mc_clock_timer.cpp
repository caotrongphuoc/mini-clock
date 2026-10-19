#include "mc_clock_timer.h"

mc_clock_timer_t mc_clock_timer = {
    .set_hour = 0,
    .set_min = 1,
    .set_sec = 0,

    .remaining_hour = 0,
    .remaining_min = 1,
    .remaining_sec = 0,

    .running = 0,
    .finished = 0,

    .edit_field = TIMER_EDIT_MIN,
};
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
		if (mc_clock_timer.finished)
		{
			mc_clock_timer.finished = 0;

			mc_clock_timer.remaining_hour = mc_clock_timer.set_hour;
			mc_clock_timer.remaining_min = mc_clock_timer.set_min;
			mc_clock_timer.remaining_sec = mc_clock_timer.set_sec;

			BUZZER_Disable();
			break;
		}

		if (!mc_clock_timer.running &&
		    mc_clock_timer.remaining_hour == 0 &&
		    mc_clock_timer.remaining_min == 0 &&
		    mc_clock_timer.remaining_sec == 0)
		{
			mc_clock_timer.remaining_hour = mc_clock_timer.set_hour;
			mc_clock_timer.remaining_min = mc_clock_timer.set_min;
			mc_clock_timer.remaining_sec = mc_clock_timer.set_sec;
		}

		mc_clock_timer.running = !mc_clock_timer.running;
	}
	break;

	case MC_CLOCK_TIMER_RESET:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_RESET\n");
		mc_clock_timer.running = 0;
		mc_clock_timer.finished = 0;

		mc_clock_timer.remaining_hour = mc_clock_timer.set_hour;
		mc_clock_timer.remaining_min = mc_clock_timer.set_min;
		mc_clock_timer.remaining_sec = mc_clock_timer.set_sec;

		BUZZER_Disable();
	}
	break;

	case MC_CLOCK_TIMER_INC:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_INC\n");
		if (!mc_clock_timer.running)
		{
			mc_clock_timer.finished = 0;

			switch (mc_clock_timer.edit_field)
			{
			case TIMER_EDIT_HOUR:
				if (mc_clock_timer.set_hour < TIMER_HOUR_MAX)
					mc_clock_timer.set_hour++;
				break;

			case TIMER_EDIT_MIN:
				if (mc_clock_timer.set_min < 59)
					mc_clock_timer.set_min++;
				break;

			case TIMER_EDIT_SEC:
				if (mc_clock_timer.set_sec < 59)
					mc_clock_timer.set_sec++;
				break;
			}

			mc_clock_timer.remaining_hour = mc_clock_timer.set_hour;
			mc_clock_timer.remaining_min = mc_clock_timer.set_min;
			mc_clock_timer.remaining_sec = mc_clock_timer.set_sec;
		}
	}
	break;

	case MC_CLOCK_TIMER_DEC:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_DEC\n");
		if (!mc_clock_timer.running)
		{
			mc_clock_timer.finished = 0;

			switch (mc_clock_timer.edit_field)
			{
			case TIMER_EDIT_HOUR:
				if (mc_clock_timer.set_hour > 0)
					mc_clock_timer.set_hour--;
				break;

			case TIMER_EDIT_MIN:
				if (mc_clock_timer.set_min > 0)
					mc_clock_timer.set_min--;
				break;

			case TIMER_EDIT_SEC:
				if (mc_clock_timer.set_sec > 0)
					mc_clock_timer.set_sec--;
				break;
			}

			mc_clock_timer.remaining_hour = mc_clock_timer.set_hour;
			mc_clock_timer.remaining_min = mc_clock_timer.set_min;
			mc_clock_timer.remaining_sec = mc_clock_timer.set_sec;
		}
	}
	break;

	case MC_CLOCK_TIMER_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_UPDATE\n");
		if (!mc_clock_timer.running)
			break;

		if (mc_clock_timer.remaining_sec > 0)
		{
			mc_clock_timer.remaining_sec--;
		}
		else if (mc_clock_timer.remaining_min > 0)
		{
			mc_clock_timer.remaining_min--;
			mc_clock_timer.remaining_sec = 59;
		}
		else if (mc_clock_timer.remaining_hour > 0)
		{
			mc_clock_timer.remaining_hour--;
			mc_clock_timer.remaining_min = 59;
			mc_clock_timer.remaining_sec = 59;
		}
		else
		{
			mc_clock_timer.running = 0;
			mc_clock_timer.finished = 1;
			BUZZER_PlaySound((buzzer_sound_t)mc_clock_alarm_get_sound());
			mc_clock_ring_request(RING_SOURCE_TIMER, mc_clock_alarm_get_sound());
		}

		/* Redraw with the new value; the tick that requested it was rendered
		 * before this update ran, which made the clock show the previous second. */
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case MC_CLOCK_TIMER_DISMISS:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_DISMISS\n");
		mc_clock_timer.running = 0;
		mc_clock_timer.finished = 0;
		mc_clock_timer.remaining_hour = mc_clock_timer.set_hour;
		mc_clock_timer.remaining_min = mc_clock_timer.set_min;
		mc_clock_timer.remaining_sec = mc_clock_timer.set_sec;
		BUZZER_Disable();
	}
	break;

	case MC_CLOCK_TIMER_NEXT_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_NEXT_FIELD\n");
		if (!mc_clock_timer.running)
		{
			if (mc_clock_timer.edit_field == TIMER_EDIT_SEC)
			{
				mc_clock_timer.edit_field = TIMER_EDIT_HOUR;
			}
			else
			{
				mc_clock_timer.edit_field++;
			}
		}
	}
	break;

	case MC_CLOCK_TIMER_PREV_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_PREV_FIELD\n");
		if (!mc_clock_timer.running)
		{
			if (mc_clock_timer.edit_field == TIMER_EDIT_HOUR)
			{
				mc_clock_timer.edit_field = TIMER_EDIT_SEC;
			}
			else
			{
				mc_clock_timer.edit_field--;
			}
		}
	}
	break;

	default:
		break;
	}
}
