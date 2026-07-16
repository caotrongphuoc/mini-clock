#include "mc_clock_alarm.h"
#include "mc_clock_world.h"

/* Variable Declaration - Clock alarm object */
static mc_clock_alarm_state_t mc_clock_alarm_state = {
    .current_item = 0,
    .scroll_offset = 0,
    .total_alarm = 3,
    .editing = 0,
    .editing_alarm = 0,
    .editing_field = 0,
    .ringing = 0,
    .alarm_sound = BUZZER_SOUND_ALARM_CLASSIC,
    .alarm = {
        {7, 30, 1},
        {8, 0, 1},
        {12, 15, 0},
    },
};

void mc_clock_alarm_set_sound(uint8_t sound)
{
	mc_clock_alarm_state.alarm_sound = sound;
}

uint8_t mc_clock_alarm_get_sound(void)
{
	return mc_clock_alarm_state.alarm_sound;
}

/* Public API - Clock alarm object */
void mc_clock_alarm_get_state(mc_clock_alarm_state_t* state)
{
	*state = mc_clock_alarm_state;
}

void mc_clock_alarm_apply_rtc(void)
{
	rtc_time_t now;
	uint16_t now_min;
	uint16_t best_delta = 24 * 60;
	uint16_t best_a_min_rtc = 0;
	uint8_t found = 0;

	rtc_get_time(&now);
	now_min = now.hour * 60 + now.min;

	int16_t offset_minutes = mc_clock_world_clock_get_selected_offset_minutes();

	for (uint8_t i = 0; i < mc_clock_alarm_state.total_alarm; i++)
	{
		if (!mc_clock_alarm_state.alarm[i].enabled)
		{
			continue;
		}

		// Convert local country alarm time to base RTC timezone time
		int32_t a_min_rtc = (int32_t)mc_clock_alarm_state.alarm[i].hour * 60 + mc_clock_alarm_state.alarm[i].minute - offset_minutes;
		a_min_rtc = ((a_min_rtc % 1440) + 1440) % 1440;

		uint16_t delta = (a_min_rtc > now_min) ? (a_min_rtc - now_min) : (24 * 60 - (now_min - a_min_rtc));

		if (!found || delta < best_delta)
		{
			found = 1;
			best_delta = delta;
			best_a_min_rtc = (uint16_t)a_min_rtc;
		}
	}

	if (!found)
	{
		task_post_pure_msg(MC_CLOCK_RTC_ID, MC_CLOCK_RTC_CLEAR_ALARM_REQ);
		return;
	}

	mc_clock_rtc_set_alarm_req_t req;
	req.time.hour = (uint8_t)(best_a_min_rtc / 60);
	req.time.min = (uint8_t)(best_a_min_rtc % 60);
	req.time.sec = 0;
	req.weekday = RTC_ALARM_ANY_WEEKDAY;

	task_post_common_msg(MC_CLOCK_RTC_ID,
	                     MC_CLOCK_RTC_SET_ALARM_REQ,
	                     (uint8_t*)&req,
	                     sizeof(req));
}

static void mc_clock_alarm_scroll_to_current(void)
{
    if (mc_clock_alarm_state.current_item >= mc_clock_alarm_state.total_alarm)
    {
        return;
    }

    if (mc_clock_alarm_state.current_item < mc_clock_alarm_state.scroll_offset)
    {
        mc_clock_alarm_state.scroll_offset = mc_clock_alarm_state.current_item;
    }
    else if (mc_clock_alarm_state.current_item >= mc_clock_alarm_state.scroll_offset + 3)
    {
        mc_clock_alarm_state.scroll_offset = mc_clock_alarm_state.current_item - 2;
    }
}

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
			// Finish editing
			mc_clock_alarm_state.editing = 0;
			mc_clock_alarm_apply_rtc();
			break;
		}

		// Add New button
		if (mc_clock_alarm_state.current_item == mc_clock_alarm_state.total_alarm)
		{
			if (mc_clock_alarm_state.total_alarm < MC_CLOCK_ALARM_MAX)
			{
				uint8_t new_alarm = mc_clock_alarm_state.total_alarm;

				mc_clock_alarm_state.alarm[new_alarm].hour = 8;
				mc_clock_alarm_state.alarm[new_alarm].minute = 0;
				mc_clock_alarm_state.alarm[new_alarm].enabled = 1;

				mc_clock_alarm_state.total_alarm++;

				// Select the newly created alarm
				mc_clock_alarm_state.current_item = new_alarm;

				mc_clock_alarm_apply_rtc();
			}

			break;
		}

		// Back button
		if (mc_clock_alarm_state.current_item == mc_clock_alarm_state.total_alarm + 1)
		{
			break;
		}

		// Existing alarm selected
		if (mc_clock_alarm_state.current_item < mc_clock_alarm_state.total_alarm)
		{
			mc_clock_alarm_state.editing = 1;
			mc_clock_alarm_state.editing_alarm = mc_clock_alarm_state.current_item;
			mc_clock_alarm_state.editing_field = 0;
		}
	}
	break;

	case MC_CLOCK_ALARM_UP:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_UP\n");
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

		mc_clock_alarm_scroll_to_current();
	}
	break;

	case MC_CLOCK_ALARM_DOWN:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_DOWN\n");
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

		mc_clock_alarm_scroll_to_current();
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

	case MC_CLOCK_ALARM_TOGGLE:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_TOGGLE\n");
		if (mc_clock_alarm_state.editing == 0 &&
		    mc_clock_alarm_state.current_item < mc_clock_alarm_state.total_alarm)
		{
			mc_clock_alarm_item_t* alarm = &mc_clock_alarm_state.alarm[mc_clock_alarm_state.current_item];
			alarm->enabled = !alarm->enabled;
			mc_clock_alarm_apply_rtc();
		}
	}
	break;

	case MC_CLOCK_ALARM_DELETE:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_DELETE\n");
		if (mc_clock_alarm_state.editing == 0 &&
		    mc_clock_alarm_state.current_item < mc_clock_alarm_state.total_alarm)
		{
			for (uint8_t i = mc_clock_alarm_state.current_item; i + 1 < mc_clock_alarm_state.total_alarm; i++)
			{
				mc_clock_alarm_state.alarm[i] = mc_clock_alarm_state.alarm[i + 1];
			}

			mc_clock_alarm_state.total_alarm--;
			if (mc_clock_alarm_state.current_item > mc_clock_alarm_state.total_alarm + 1)
			{
				mc_clock_alarm_state.current_item = mc_clock_alarm_state.total_alarm;
			}
			mc_clock_alarm_apply_rtc();
		}
	}
	break;

	case MC_CLOCK_ALARM_FIRED:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_FIRED\n");
		mc_clock_alarm_state.ringing = 1;
		BUZZER_PlaySound((buzzer_sound_t)mc_clock_alarm_state.alarm_sound);
	}
	break;

	case MC_CLOCK_ALARM_DISMISS:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_DISMISS\n");
		mc_clock_alarm_state.ringing = 0;
		BUZZER_Disable();
		mc_clock_alarm_apply_rtc();
	}
	break;

	default:
		break;
	}
}
