#include "mc_clock_calender.h"

#include "rtc.h"
#include "buzzer.h"
#include "mc_clock_time.h"
#include "mc_clock_world_clock.h"
#include "app_dbg.h"
#include "task_list.h"

#include <string.h>

/* Category names                                                             */
const char* const MC_CAL_CAT_NAME[MC_CAL_CAT_MAX] = {
    "Work",
    "Meeting",
    "Personal",
    "Reminder",
};

/* Variable Declaration                                                       */
static mc_calendar_state_t calendar;

/* Helper - days in month (leap year aware)                                  */
uint8_t mc_calendar_days_in_month(uint8_t month, uint16_t year)
{
	static const uint8_t days_table[13] =
	    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (month < 1 || month > 12)
	{
		return 30;
	}

	if (month == 2)
	{
		uint8_t is_leap = ((year % 4 == 0) &&
		                   (year % 100 != 0 || year % 400 == 0))
		                      ? 1
		                      : 0;
		return is_leap ? 29 : 28;
	}

	return days_table[month];
}

/* Helper - check if any event on a day*/
uint8_t mc_calendar_has_event_on_day(
    uint16_t year, uint8_t month, uint8_t day)
{
	for (uint8_t i = 0; i < calendar.total_events; i++)
	{
		if (calendar.events[i].year == year &&
		    calendar.events[i].month == month &&
		    calendar.events[i].day == day)
		{
			return 1;
		}
	}
	return 0;
}

/* Helper - sync today from RTC                                               */
static void mc_calendar_sync_today(void)
{
	rtc_time_t raw_time;
	rtc_date_t raw_date;
	rtc_time_t local_time;
	rtc_date_t local_date;
	rtc_get_time(&raw_time);
	rtc_get_date(&raw_date);
	mc_clock_time_adjust_timezone(&raw_time, &raw_date, mc_clock_world_clock_get_selected_offset_minutes(), &local_time, &local_date);
	calendar.today_year = local_date.year;
	calendar.today_month = local_date.month;
	calendar.today_day = local_date.date;
}

/* Helper - clamp selected_day to valid range for current view month         */
static void mc_calendar_clamp_day(void)
{
	uint8_t max_day = mc_calendar_days_in_month(
	    calendar.view_month, calendar.view_year);
	if (calendar.selected_day < 1)
	{
		calendar.selected_day = 1;
	}
	if (calendar.selected_day > max_day)
	{
		calendar.selected_day = max_day;
	}
}

/* Helper - scroll event list to keep current_event visible                  */
static void mc_calendar_scroll_to_event(void)
{
	if (calendar.current_event < calendar.scroll_offset)
	{
		calendar.scroll_offset = calendar.current_event;
	}
	else if (calendar.current_event >= calendar.scroll_offset + 3)
	{
		calendar.scroll_offset = calendar.current_event - 2;
	}
}

/* Helper - increment / decrement a field value                              */
static void mc_calendar_field_change(int8_t delta)
{
	if (calendar.editing_event >= calendar.total_events)
	{
		return;
	}

	mc_calendar_event_t* ev = &calendar.events[calendar.editing_event];
	uint8_t days;

	switch (calendar.editing_field)
	{
	case MC_CAL_FIELD_YEAR:
		if (delta > 0)
		{
			if (ev->year < 2099)
				ev->year++;
		}
		else
		{
			if (ev->year > 2000)
				ev->year--;
		}
		/* Re-clamp day */
		days = mc_calendar_days_in_month(ev->month, ev->year);
		if (ev->day > days)
			ev->day = days;
		break;

	case MC_CAL_FIELD_MONTH:
		if (delta > 0)
		{
			ev->month = (ev->month % 12) + 1;
		}
		else
		{
			ev->month = (ev->month == 1) ? 12 : ev->month - 1;
		}
		days = mc_calendar_days_in_month(ev->month, ev->year);
		if (ev->day > days)
			ev->day = days;
		break;

	case MC_CAL_FIELD_DAY:
		days = mc_calendar_days_in_month(ev->month, ev->year);
		if (delta > 0)
		{
			ev->day = (ev->day % days) + 1;
		}
		else
		{
			ev->day = (ev->day == 1) ? days : ev->day - 1;
		}
		break;

	case MC_CAL_FIELD_CATEGORY:
		if (delta > 0)
		{
			ev->category = (ev->category + 1) % MC_CAL_CAT_MAX;
		}
		else
		{
			ev->category = (ev->category == 0)
			                   ? (MC_CAL_CAT_MAX - 1)
			                   : (ev->category - 1);
		}
		break;

	case MC_CAL_FIELD_START_HOUR:
		if (delta > 0)
		{
			ev->start_hour = (ev->start_hour + 1) % 24;
		}
		else
		{
			ev->start_hour = (ev->start_hour == 0) ? 23 : ev->start_hour - 1;
		}
		break;

	case MC_CAL_FIELD_START_MIN:
		if (delta > 0)
		{
			ev->start_min = (ev->start_min + 1) % 60;
		}
		else
		{
			ev->start_min = (ev->start_min == 0) ? 59 : ev->start_min - 1;
		}
		break;

	case MC_CAL_FIELD_END_HOUR:
		if (delta > 0)
		{
			ev->end_hour = (ev->end_hour + 1) % 24;
		}
		else
		{
			ev->end_hour = (ev->end_hour == 0) ? 23 : ev->end_hour - 1;
		}
		break;

	case MC_CAL_FIELD_END_MIN:
		if (delta > 0)
		{
			ev->end_min = (ev->end_min + 1) % 60;
		}
		else
		{
			ev->end_min = (ev->end_min == 0) ? 59 : ev->end_min - 1;
		}
		break;

	case MC_CAL_FIELD_ALARM:
		ev->alarm_enabled = !ev->alarm_enabled;
		break;

	default:
		break;
	}
}

void mc_clock_calendar_init(void)
{
	memset(&calendar, 0, sizeof(calendar));

	rtc_date_t date;
	rtc_get_date(&date);

	calendar.today_year = date.year;
	calendar.today_month = date.month;
	calendar.today_day = date.date;

	calendar.view_year = date.year;
	calendar.view_month = date.month;
	calendar.selected_day = date.date;

	calendar.total_events = 0;
	calendar.mode = MC_CAL_MODE_MONTH;
}

void mc_clock_calendar_get_state(mc_calendar_state_t* state)
{
	memcpy(state, &calendar, sizeof(mc_calendar_state_t));
}

void mc_clock_calendar_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_CALENDAR_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_SETUP\n");

		mc_calendar_sync_today();

		/* If view has never been set, jump to today */
		if (calendar.view_year == 0)
		{
			calendar.view_year = calendar.today_year;
			calendar.view_month = calendar.today_month;
			calendar.selected_day = calendar.today_day;
		}

		calendar.mode = MC_CAL_MODE_MONTH;
	}
	break;

	/* 1-second tick: update today's date and check event alarms           */
	case MC_CLOCK_CALENDAR_UPDATE:
	{
		mc_calendar_sync_today();

		if (calendar.ringing)
		{
			break; /* Already ringing — wait for dismiss */
		}

		rtc_time_t now;
		rtc_get_time(&now);

		for (uint8_t i = 0; i < calendar.total_events; i++)
		{
			mc_calendar_event_t* ev = &calendar.events[i];

			if (!ev->alarm_enabled || ev->alarm_fired)
			{
				continue;
			}

			if (ev->year == calendar.today_year &&
			    ev->month == calendar.today_month &&
			    ev->day == calendar.today_day &&
			    ev->start_hour == now.hour &&
			    ev->start_min == now.min &&
			    now.sec == 0)
			{
				APP_DBG_SIG("MC_CLOCK_CALENDAR_UPDATE — alarm fired for event %u\n", i);
				ev->alarm_fired = 1;
				calendar.ringing = 1;
				calendar.ringing_event = i;
				task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_ALARM_FIRED);
				break;
			}
		}
	}
	break;

	case MC_CLOCK_CALENDAR_ALARM_FIRED:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_ALARM_FIRED\n");
		calendar.ringing = 1;
		BUZZER_PlaySound(BUZZER_SOUND_GOODBYE);
	}
	break;

	case MC_CLOCK_CALENDAR_ALARM_DISMISS:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_ALARM_DISMISS\n");
		calendar.ringing = 0;
		BUZZER_Disable();
	}
	break;

	case MC_CLOCK_CALENDAR_NEXT_MONTH:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_NEXT_MONTH\n");
		if (calendar.view_month == 12)
		{
			calendar.view_month = 1;
			calendar.view_year++;
		}
		else
		{
			calendar.view_month++;
		}
		mc_calendar_clamp_day();
	}
	break;

	case MC_CLOCK_CALENDAR_PREV_MONTH:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_PREV_MONTH\n");
		if (calendar.view_month == 1)
		{
			calendar.view_month = 12;
			calendar.view_year--;
		}
		else
		{
			calendar.view_month--;
		}
		mc_calendar_clamp_day();
	}
	break;

	/* Add a new event pre-filled with the currently selected date         */
	case MC_CLOCK_CALENDAR_ADD_EVENT:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_ADD_EVENT\n");

		if (calendar.total_events >= MC_CALENDAR_MAX_EVENTS)
		{
			break;
		}

		uint8_t idx = calendar.total_events;
		memset(&calendar.events[idx], 0, sizeof(mc_calendar_event_t));

		calendar.events[idx].year = calendar.view_year;
		calendar.events[idx].month = calendar.view_month;
		calendar.events[idx].day = calendar.selected_day;
		calendar.events[idx].start_hour = 8;
		calendar.events[idx].start_min = 0;
		calendar.events[idx].end_hour = 9;
		calendar.events[idx].end_min = 0;
		calendar.events[idx].category = MC_CAL_CAT_WORK;
		calendar.events[idx].alarm_enabled = 1;
		calendar.events[idx].alarm_fired = 0;

		calendar.total_events++;
		calendar.editing_event = idx;
		calendar.editing_field = MC_CAL_FIELD_YEAR;
		calendar.is_new_event = 1;
		calendar.mode = MC_CAL_MODE_EDIT;
	}
	break;

	/* From MONTH view: switch to event LIST for selected day.             */
	/* From LIST view: enter EDIT mode for the selected event.             */
	case MC_CLOCK_CALENDAR_EDIT_EVENT:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_EDIT_EVENT\n");

		if (calendar.mode == MC_CAL_MODE_MONTH)
		{
			/* Find first event on selected day */
			calendar.current_event = 255;

			for (uint8_t i = 0; i < calendar.total_events; i++)
			{
				if (calendar.events[i].year == calendar.view_year &&
				    calendar.events[i].month == calendar.view_month &&
				    calendar.events[i].day == calendar.selected_day)
				{
					calendar.current_event = i;
					break;
				}
			}

			calendar.scroll_offset = 0;
			calendar.mode = MC_CAL_MODE_LIST;
		}
		else if (calendar.mode == MC_CAL_MODE_LIST)
		{
			if (calendar.current_event != 255 &&
			    calendar.current_event < calendar.total_events)
			{
				calendar.editing_event = calendar.current_event;
				calendar.editing_field = MC_CAL_FIELD_YEAR;
				calendar.is_new_event = 0;
				calendar.mode = MC_CAL_MODE_EDIT;
			}
		}
	}
	break;

	case MC_CLOCK_CALENDAR_DELETE_EVENT:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_DELETE_EVENT\n");

		uint8_t del;

		if (calendar.mode == MC_CAL_MODE_EDIT)
			del = calendar.editing_event;
		else
			del = calendar.current_event;

		if (del < calendar.total_events)
		{
			for (uint8_t i = del; i + 1 < calendar.total_events; i++)
			{
				calendar.events[i] = calendar.events[i + 1];
			}

			calendar.total_events--;

			if (calendar.total_events == 0)
			{
				calendar.current_event = 255;
				calendar.mode = MC_CAL_MODE_MONTH;
				break;
			}

			if (calendar.current_event >= calendar.total_events)
				calendar.current_event = calendar.total_events - 1;

			mc_calendar_scroll_to_event();
		}
	}
	break;

	case MC_CLOCK_CALENDAR_NEXT_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_NEXT_FIELD\n");

		if (calendar.mode == MC_CAL_MODE_EDIT)
		{
			if (calendar.editing_field + 1 < MC_CAL_FIELD_MAX)
			{
				calendar.editing_field++;
			}
			else
			{
				/* Last field confirmed — reset alarm_fired so alarm can fire */
				calendar.events[calendar.editing_event].alarm_fired = 0;
				calendar.mode = MC_CAL_MODE_LIST;
				calendar.current_event = calendar.editing_event;
				calendar.scroll_offset = 0;
				mc_calendar_scroll_to_event();
			}
		}
	}
	break;

	/* ------------------------------------------------------------------ */
	case MC_CLOCK_CALENDAR_FIELD_INC:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_FIELD_INC\n");

		if (calendar.mode == MC_CAL_MODE_EDIT)
		{
			mc_calendar_field_change(+1);
		}
		else if (calendar.mode == MC_CAL_MODE_MONTH)
		{
			/* In month view UP increments selected day */
			uint8_t max_day = mc_calendar_days_in_month(
			    calendar.view_month, calendar.view_year);
			if (calendar.selected_day < max_day)
			{
				calendar.selected_day++;
			}
			else
			{
				/* Move to next month */
				task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_NEXT_MONTH);
				calendar.selected_day = 1;
			}
		}
		else if (calendar.mode == MC_CAL_MODE_LIST)
		{
			if (calendar.current_event + 1 < calendar.total_events)
			{
				calendar.current_event++;
				mc_calendar_scroll_to_event();
			}
		}
	}
	break;

	/* ------------------------------------------------------------------ */
	case MC_CLOCK_CALENDAR_FIELD_DEC:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_FIELD_DEC\n");

		if (calendar.mode == MC_CAL_MODE_EDIT)
		{
			mc_calendar_field_change(-1);
		}
		else if (calendar.mode == MC_CAL_MODE_MONTH)
		{
			if (calendar.selected_day > 1)
			{
				calendar.selected_day--;
			}
			else
			{
				/* Move to previous month */
				task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_PREV_MONTH);
				calendar.selected_day =
				    mc_calendar_days_in_month(calendar.view_month, calendar.view_year);
			}
		}
		else if (calendar.mode == MC_CAL_MODE_LIST)
		{
			if (calendar.current_event > 0)
			{
				calendar.current_event--;
				mc_calendar_scroll_to_event();
			}
		}
	}
	break;

	/* ------------------------------------------------------------------ */
	default:
		break;
	}
}

void mc_clock_calendar_sync(void)
{
	mc_calendar_sync_today();
}