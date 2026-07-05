#include "mc_clock_clock.h"

static mc_clock_clock_state_t s_clock_state;

static void mc_clock_clock_sync_from_rtc()
{
	rtc_get_time(&s_clock_state.time);
	rtc_get_date(&s_clock_state.date);
}

void mc_clock_clock_get_time(rtc_time_t* time)
{
	*time = s_clock_state.time;
}

void mc_clock_clock_get_date(rtc_date_t* date)
{
	*date = s_clock_state.date;
}

uint8_t mc_clock_clock_is_24h_format()
{
	return s_clock_state.format_24h;
}

void mc_clock_clock_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_CLOCK_ENTER:
	{
		s_clock_state.format_24h = 1;
		mc_clock_clock_sync_from_rtc();
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case MC_CLOCK_CLOCK_TICK:
	{
		mc_clock_clock_sync_from_rtc();
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case MC_CLOCK_CLOCK_FORMAT_TOGGLE:
	{
		s_clock_state.format_24h = !s_clock_state.format_24h;
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case MC_CLOCK_CLOCK_LEAVE:
	default:
		break;
	}
}
