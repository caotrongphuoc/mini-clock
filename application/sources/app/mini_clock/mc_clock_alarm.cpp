#include "mc_clock_alarm.h"

#include "app_dbg.h"

/*****************************************************************************/
/* Variable Declaration - Clock alarm object */
/*****************************************************************************/

mc_clock_alarm_state_t mc_clock_alarm_state = {
    .current_item = 0,
    .scroll_offset = 0,
    .total_alarm = 3,
    .editing = 0,
    .editing_alarm = 0,
    .editing_field = 0,
    .alarm = {
        {7, 30, 1},
        {8, 0, 1},
        {12, 15, 0},
    },
};

/*****************************************************************************/
/* Public API - Clock alarm object */
/*****************************************************************************/

void mc_clock_alarm_get_state(mc_clock_alarm_state_t* state)
{
	*state = mc_clock_alarm_state;
}

/*****************************************************************************/
/* Handle - Clock alarm object */
/*****************************************************************************/

void mc_clock_alarm_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_ALARM_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_SETUP\n");
		mc_clock_alarm_state.current_item = 0;
		mc_clock_alarm_state.scroll_offset = 0;
		mc_clock_alarm_state.editing = 0;
		mc_clock_alarm_state.editing_field = 0;
	}
	break;

	case MC_CLOCK_ALARM_SELECT:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_SELECT\n");
		if (mc_clock_alarm_state.editing)
		{
			mc_clock_alarm_state.editing = 0;
			break;
		}

		if (mc_clock_alarm_state.current_item == mc_clock_alarm_state.total_alarm)
		{
			if (mc_clock_alarm_state.total_alarm < MC_CLOCK_ALARM_MAX)
			{
				uint8_t new_alarm = mc_clock_alarm_state.total_alarm;
				mc_clock_alarm_state.alarm[new_alarm].hour = 8;
				mc_clock_alarm_state.alarm[new_alarm].minute = 0;
				mc_clock_alarm_state.alarm[new_alarm].enabled = 1;
				mc_clock_alarm_state.total_alarm++;
				mc_clock_alarm_state.current_item = new_alarm;
			}
			break;
		}

		if (mc_clock_alarm_state.current_item < mc_clock_alarm_state.total_alarm)
		{
			mc_clock_alarm_state.editing = 1;
			mc_clock_alarm_state.editing_alarm = mc_clock_alarm_state.current_item;
			mc_clock_alarm_state.editing_field = 0;
		}
	}
	break;

	case MC_CLOCK_ALARM_PREV:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_PREV\n");
		if (mc_clock_alarm_state.editing)
		{
			mc_clock_alarm_item_t* alarm = &mc_clock_alarm_state.alarm[mc_clock_alarm_state.editing_alarm];
			if (mc_clock_alarm_state.editing_field == 0)
			{
				alarm->hour = (alarm->hour + 1) % 24;
			}
			else
			{
				alarm->minute = (alarm->minute + 1) % 60;
			}
			break;
		}

		if (mc_clock_alarm_state.current_item == 0)
		{
			mc_clock_alarm_state.current_item = mc_clock_alarm_state.total_alarm + 1;
		}
		else
		{
			mc_clock_alarm_state.current_item--;
		}

		if (mc_clock_alarm_state.current_item < mc_clock_alarm_state.scroll_offset)
		{
			mc_clock_alarm_state.scroll_offset = mc_clock_alarm_state.current_item;
		}
	}
	break;

	case MC_CLOCK_ALARM_NEXT:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_NEXT\n");
		if (mc_clock_alarm_state.editing)
		{
			mc_clock_alarm_item_t* alarm = &mc_clock_alarm_state.alarm[mc_clock_alarm_state.editing_alarm];
			if (mc_clock_alarm_state.editing_field == 0)
			{
				alarm->hour = (alarm->hour == 0) ? 23 : alarm->hour - 1;
			}
			else
			{
				alarm->minute = (alarm->minute == 0) ? 59 : alarm->minute - 1;
			}
			break;
		}

		mc_clock_alarm_state.current_item++;
		if (mc_clock_alarm_state.current_item > mc_clock_alarm_state.total_alarm + 1)
		{
			mc_clock_alarm_state.current_item = 0;
		}

		if (mc_clock_alarm_state.current_item >= mc_clock_alarm_state.scroll_offset + 3)
		{
			mc_clock_alarm_state.scroll_offset = mc_clock_alarm_state.current_item - 2;
		}
	}
	break;

	case MC_CLOCK_ALARM_NEXT_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_NEXT_FIELD\n");
		if (mc_clock_alarm_state.editing)
		{
			mc_clock_alarm_state.editing_field = !mc_clock_alarm_state.editing_field;
		}
	}
	break;

	default:
		break;
	}
}
