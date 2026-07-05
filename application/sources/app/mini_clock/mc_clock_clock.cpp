#include "mc_clock_clock.h"

static mc_clock_clock_state_t s_clock_state;

void mc_clock_clock_get_state(mc_clock_clock_state_t* state)
{
	*state = s_clock_state;
}

void mc_clock_clock_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_CLOCK_ENTER:
	{
		s_clock_state.format_24h = 1;
		rtc_get_time(&s_clock_state.time);
		rtc_get_date(&s_clock_state.date);
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case MC_CLOCK_CLOCK_TICK:
	{
		rtc_get_time(&s_clock_state.time);
		rtc_get_date(&s_clock_state.date);
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
