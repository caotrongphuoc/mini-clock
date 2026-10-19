#include "mc_clock_calendar.h"

#include "mc_clock_alarm.h"
#include "eeprom.h"
#include "app_eeprom.h"
#include "mc_clock_ring.h"

#define MC_CALENDAR_CHECKSUM_SIZE (sizeof(uint32_t) + sizeof(mc_calendar_bank_t))

typedef struct
{
	uint32_t           magic_number;
	mc_calendar_bank_t data;
	uint8_t            check_sum;
} mc_calendar_eeprom_t;

const char* const MC_CAL_REPEAT_NAME[MC_CAL_REPEAT_MAX] =
    {
        "None",
        "Daily",
        "Weekly",
        "Monthly",
        "Yearly"};

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

/* The list view shows only the selected day's events, MC_CAL_LIST_VISIBLE at a
 * time, and scroll_offset counts positions within that day. Navigation below
 * walks the same filtered list so the cursor never lands on a hidden event. */
#define MC_CAL_LIST_VISIBLE (2)

/* Copy of the event being edited, restored when an edit is abandoned */
static mc_calendar_event_t calendar_edit_backup;

static uint8_t mc_calendar_on_selected_day(uint8_t idx)
{
	const mc_calendar_event_t* ev = &calendar.events[idx];

	return ev->year == calendar.view_year &&
	       ev->month == calendar.view_month &&
	       ev->day == calendar.selected_day;
}

/* Next (dir > 0) or previous (dir < 0) selected-day event from idx, or 255 */
static uint8_t mc_calendar_find_on_day(int16_t idx, int8_t dir)
{
	for (idx += dir; idx >= 0 && idx < calendar.total_events; idx += dir)
	{
		if (mc_calendar_on_selected_day((uint8_t)idx))
		{
			return (uint8_t)idx;
		}
	}

	return 255;
}

/* Helper - scroll event list to keep current_event visible                  */
static void mc_calendar_scroll_to_event(void)
{
	if (calendar.current_event >= calendar.total_events)
	{
		calendar.scroll_offset = 0;
		return;
	}

	uint8_t pos = 0;

	for (uint8_t i = 0; i < calendar.current_event; i++)
	{
		if (mc_calendar_on_selected_day(i))
		{
			pos++;
		}
	}

	if (pos < calendar.scroll_offset)
	{
		calendar.scroll_offset = pos;
	}
	else if (pos >= calendar.scroll_offset + MC_CAL_LIST_VISIBLE)
	{
		calendar.scroll_offset = pos - (MC_CAL_LIST_VISIBLE - 1);
	}
}

/* Move the month view by one month; the caller picks the selected day */
static void mc_calendar_shift_month(int8_t dir)
{
	if (dir > 0)
	{
		if (calendar.view_month == 12)
		{
			calendar.view_month = 1;
			calendar.view_year++;
		}
		else
		{
			calendar.view_month++;
		}
	}
	else
	{
		if (calendar.view_month == 1)
		{
			calendar.view_month = 12;
			calendar.view_year--;
		}
		else
		{
			calendar.view_month--;
		}
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

void mc_clock_calendar_save_to_eeprom(void)
{
	mc_calendar_eeprom_t eeprom_data;

	eeprom_data.data.total_events = calendar.total_events;
	for (uint8_t i = 0; i < MC_CALENDAR_MAX_EVENTS; i++)
	{
		eeprom_data.data.events[i] = calendar.events[i];
	}

	mc_clock_eeprom_update_checksum(&eeprom_data.magic_number,
	                                &eeprom_data.check_sum,
	                                MC_CALENDAR_CHECKSUM_SIZE);

	eeprom_write(EEPROM_CALENDAR_START_ADDR,
	             (uint8_t*)&eeprom_data,
	             sizeof(eeprom_data));
}

void mc_clock_calendar_load_from_eeprom(void)
{
	mc_calendar_eeprom_t eeprom_data;

	uint8_t ret = eeprom_read(EEPROM_CALENDAR_START_ADDR,
	                          (uint8_t*)&eeprom_data,
	                          sizeof(eeprom_data));

	if (ret == EEPROM_DRIVER_OK &&
	    mc_clock_eeprom_is_valid(&eeprom_data.magic_number,
	                             eeprom_data.check_sum,
	                             MC_CALENDAR_CHECKSUM_SIZE))
	{
		calendar.total_events = eeprom_data.data.total_events;
		if (calendar.total_events > MC_CALENDAR_MAX_EVENTS)
		{
			calendar.total_events = 0;
		}
		for (uint8_t i = 0; i < MC_CALENDAR_MAX_EVENTS; i++)
		{
			calendar.events[i] = eeprom_data.data.events[i];
		}
	}
}

void mc_clock_calendar_get_state(mc_calendar_state_t* state)
{
	if (state == NULL)
	{
		return;
	}

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

		if (calendar.view_year == 0)
		{
			calendar.view_year = calendar.today_year;
			calendar.view_month = calendar.today_month;
			calendar.selected_day = calendar.today_day;
		}

		calendar.mode = MC_CAL_MODE_MONTH;
		calendar.current_event = 255;
		calendar.editing_event = 255;
		calendar.editing_field = MC_CAL_FIELD_YEAR;
		calendar.scroll_offset = 0;
		calendar.is_new_event = 0;
		calendar.ringing = 0;


	}
	break;

	/* 1-second tick: update today's date and check event alarms           */
	case MC_CLOCK_CALENDAR_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_UPDATE\n");
		mc_calendar_sync_today();

		if (calendar.ringing)
		{
			break; /* Already ringing — wait for dismiss */
		}

		rtc_time_t raw_time;
		rtc_date_t raw_date;
		rtc_time_t local_time;
		rtc_date_t local_date;

		rtc_get_time(&raw_time);
		rtc_get_date(&raw_date);

		mc_clock_time_adjust_timezone(
		    &raw_time,
		    &raw_date,
		    mc_clock_world_clock_get_selected_offset_minutes(),
		    &local_time,
		    &local_date);

		/* Driven by the 1 Hz RTC tick on every screen. Match the whole start
		 * minute (a tick can skip second 0) and fire once per occurrence. */
		for (uint8_t i = 0; i < calendar.total_events; i++)
		{
			mc_calendar_event_t* ev = &calendar.events[i];

			uint8_t due = ev->alarm_enabled &&
			              ev->year == calendar.today_year &&
			              ev->month == calendar.today_month &&
			              ev->day == calendar.today_day &&
			              ev->start_hour == local_time.hour &&
			              ev->start_min == local_time.min;

			if (!due)
			{
				ev->alarm_fired = 0;
				continue;
			}

			if (!ev->alarm_fired)
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
		BUZZER_PlaySound((buzzer_sound_t)mc_clock_alarm_get_sound());
		mc_clock_ring_request(MC_CLOCK_RING_SOURCE_CALENDAR, mc_clock_alarm_get_sound());
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
		mc_calendar_shift_month(+1);
		mc_calendar_clamp_day();
	}
	break;

	case MC_CLOCK_CALENDAR_PREV_MONTH:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_PREV_MONTH\n");
		mc_calendar_shift_month(-1);
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
		APP_DBG_SIG("EDIT EVENT MODE=%u CURRENT=%u TOTAL=%u\n",
		            calendar.mode,
		            calendar.current_event,
		            calendar.total_events);

		if (calendar.mode == MC_CAL_MODE_MONTH)
		{
			calendar.current_event = mc_calendar_find_on_day(-1, +1);
			calendar.scroll_offset = 0;
			calendar.mode = MC_CAL_MODE_LIST;

		}
		else if (calendar.mode == MC_CAL_MODE_LIST)
		{
			if (calendar.current_event >= calendar.total_events)
			{
				break;
			}

			calendar.editing_event = calendar.current_event;
			calendar.editing_field = MC_CAL_FIELD_YEAR;
			calendar.is_new_event = 0;
			calendar_edit_backup = calendar.events[calendar.editing_event];

			calendar.mode = MC_CAL_MODE_EDIT;

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

			/* Select the next event of the same day (it slid into del), else
			 * the previous one; with none left, go back to the month view. */
			calendar.current_event = mc_calendar_find_on_day((int16_t)del - 1, +1);
			if (calendar.current_event == 255)
			{
				calendar.current_event = mc_calendar_find_on_day(del, -1);
			}

			calendar.is_new_event = 0;

			if (calendar.current_event == 255)
			{
				calendar.mode = MC_CAL_MODE_MONTH;
			}
			else
			{
				calendar.mode = MC_CAL_MODE_LIST;
			}

			mc_calendar_scroll_to_event();
			mc_clock_calendar_save_to_eeprom();
		}
	}
	break;

	case MC_CLOCK_CALENDAR_NEXT_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_NEXT_FIELD\n");

		if (calendar.mode != MC_CAL_MODE_EDIT)
		{
			break;
		}

		if (calendar.editing_field < (MC_CAL_FIELD_MAX - 1))
		{
			/* Move to next editable field */
			calendar.editing_field++;
		}
		else
		{
			/* Finished editing */

			mc_calendar_event_t* ev = &calendar.events[calendar.editing_event];
			ev->alarm_fired = 0;

			calendar.view_year = ev->year;
			calendar.view_month = ev->month;
			calendar.selected_day = ev->day;

			calendar.current_event = calendar.editing_event;
			calendar.scroll_offset = 0;
			mc_calendar_scroll_to_event();

			if (calendar.is_new_event)
			{
				calendar.is_new_event = 0;
				calendar.mode = MC_CAL_MODE_MONTH;
			}
			else
			{
				calendar.mode = MC_CAL_MODE_LIST;
			}

			mc_clock_calendar_save_to_eeprom();
		}
	}
	break;

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
				mc_calendar_shift_month(+1);
				calendar.selected_day = 1;
			}
		}
		else if (calendar.mode == MC_CAL_MODE_LIST)
		{
			uint8_t next = mc_calendar_find_on_day(calendar.current_event, +1);
			if (calendar.current_event < calendar.total_events && next != 255)
			{
				calendar.current_event = next;
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
				/* Last day of the previous month */
				mc_calendar_shift_month(-1);
				calendar.selected_day =
				    mc_calendar_days_in_month(calendar.view_month, calendar.view_year);
			}
		}
		else if (calendar.mode == MC_CAL_MODE_LIST)
		{
			if (calendar.current_event >= calendar.total_events)
			{
				/* no event on this day -> create new event */
				task_post_pure_msg(
				    MC_CLOCK_CALENDAR_ID,
				    MC_CLOCK_CALENDAR_ADD_EVENT);

				break;
			}

			uint8_t prev = mc_calendar_find_on_day(calendar.current_event, -1);
			if (prev != 255)
			{
				calendar.current_event = prev;
				mc_calendar_scroll_to_event();
			}
		}
	}
	break;

	case MC_CLOCK_CALENDAR_BACK:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_BACK\n");

		if (calendar.mode == MC_CAL_MODE_LIST)
		{
			calendar.mode = MC_CAL_MODE_MONTH;
		}
		else if (calendar.mode == MC_CAL_MODE_EDIT)
		{
			if (calendar.is_new_event)
			{
				/* Abandoned new event: drop it (it was never saved) */
				calendar.total_events--;
				calendar.is_new_event = 0;
				calendar.current_event = mc_calendar_find_on_day(-1, +1);
			}
			else if (calendar.editing_event < calendar.total_events)
			{
				calendar.events[calendar.editing_event] = calendar_edit_backup;
			}

			calendar.mode = MC_CAL_MODE_LIST;
			mc_calendar_scroll_to_event();
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