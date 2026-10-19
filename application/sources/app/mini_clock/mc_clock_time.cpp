#include "mc_clock_time.h"

#include "buzzer.h"
#include "mc_clock_world.h"
#include "mc_clock_date.h"
static mc_clock_time_state_t clock_state;
static uint8_t clock_chime_enabled;

/* Public API - Clock object */
void mc_clock_time_get_state(mc_clock_time_state_t* state)
{
	*state = clock_state;
}

void mc_clock_time_set_chime_enabled(uint8_t enabled)
{
	clock_chime_enabled = enabled ? 1 : 0;
}

void mc_clock_time_adjust_timezone(const rtc_time_t* in_time, const rtc_date_t* in_date, int16_t offset_minutes, rtc_time_t* out_time, rtc_date_t* out_date)
{
	*out_time = *in_time;
	*out_date = *in_date;

	int32_t total_minutes = (int32_t)in_time->hour * 60 + in_time->min + offset_minutes;

	if (total_minutes >= 1440)
	{
		// Shift forward by 1 day
		total_minutes -= 1440;
		out_date->date++;
		out_date->weekday = (out_date->weekday == 7) ? 1 : out_date->weekday + 1;

		uint8_t max_days = mc_clock_date_days_in_month(out_date->year, out_date->month);
		if (out_date->date > max_days)
		{
			out_date->date = 1;
			out_date->month++;
			if (out_date->month > 12)
			{
				out_date->month = 1;
				out_date->year++;
			}
		}
	}
	else if (total_minutes < 0)
	{
		// Shift backward by 1 day
		total_minutes += 1440;
		out_date->weekday = (out_date->weekday == 1) ? 7 : out_date->weekday - 1;

		if (out_date->date > 1)
		{
			out_date->date--;
		}
		else
		{
			out_date->month--;
			if (out_date->month < 1)
			{
				out_date->month = 12;
				out_date->year--;
			}
			out_date->date = mc_clock_date_days_in_month(out_date->year, out_date->month);
		}
	}

	out_time->hour = (uint8_t)(total_minutes / 60);
	out_time->min = (uint8_t)(total_minutes % 60);
}

void mc_clock_time_update_local(void)
{
	rtc_time_t raw_time;
	rtc_date_t raw_date;
	rtc_get_time(&raw_time);
	rtc_get_date(&raw_date);
	mc_clock_time_adjust_timezone(&raw_time, &raw_date, mc_clock_world_clock_get_selected_offset_minutes(), &clock_state.time, &clock_state.date);
}

void mc_clock_time_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_TIME_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_TIME_SETUP\n");
		mc_clock_time_update_local();
	}
	break;

	case MC_CLOCK_TIME_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_TIME_UPDATE\n");
		mc_clock_time_update_local();

		if (clock_chime_enabled &&
		    clock_state.time.min == 0 &&
		    clock_state.time.sec == 0)
		{
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}

		/* Redraw with the new value; the tick that requested it was rendered
		 * before this update ran, which made the clock show the previous second. */
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	default:
		break;
	}
}
