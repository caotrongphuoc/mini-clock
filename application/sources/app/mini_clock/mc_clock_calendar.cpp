#include "mc_clock_calendar.h"

#include "mc_clock_alarm.h"
#include "eeprom.h"
#include "app_eeprom.h"
#include "mc_clock_ring.h"
#include "mc_clock_date.h"

// Bytes covered by the bank checksum (magic + bank)
#define CALENDAR_CHECKSUM_SIZE (sizeof(uint32_t) + sizeof(mc_clock_calendar_bank_t))

typedef struct
{
	uint32_t magic_number;
	mc_clock_calendar_bank_t data;
	uint8_t check_sum;
} mc_clock_calendar_eeprom_t;

const char* const mc_clock_calendar_repeat_name[CALENDAR_REPEAT_MAX] =
    {
        "None",
        "Daily",
        "Weekly",
        "Monthly",
        "Yearly"};

/* Category names                                                             */
const char* const mc_clock_calendar_category_name[CALENDAR_CATEGORY_MAX] = {
    "Work",
    "Meeting",
    "Personal",
    "Reminder",
};

/* Variable Declaration                                                       */
mc_clock_calendar_t mc_clock_calendar;

/* Helper - check if any event on a day*/
uint8_t mc_clock_calendar_has_event_on_day(
    uint16_t year, uint8_t month, uint8_t day)
{
	for (uint8_t i = 0; i < mc_clock_calendar.total_events; i++)
	{
		if (mc_clock_calendar.events[i].year == year &&
		    mc_clock_calendar.events[i].month == month &&
		    mc_clock_calendar.events[i].day == day)
		{
			return 1;
		}
	}
	return 0;
}

/* Helper - sync today from RTC                                               */
static void mc_clock_calendar_sync_today(void)
{
	rtc_time_t raw_time;
	rtc_date_t raw_date;
	rtc_time_t local_time;
	rtc_date_t local_date;
	rtc_get_time(&raw_time);
	rtc_get_date(&raw_date);
	mc_clock_time_adjust_timezone(&raw_time, &raw_date, mc_clock_world_clock_get_selected_offset_minutes(), &local_time, &local_date);
	mc_clock_calendar.today_year = local_date.year;
	mc_clock_calendar.today_month = local_date.month;
	mc_clock_calendar.today_day = local_date.date;
}

/* Helper - clamp selected_day to valid range for current view month         */
static void mc_clock_calendar_clamp_day(void)
{
	uint8_t max_day = mc_clock_date_days_in_month(mc_clock_calendar.view_year, mc_clock_calendar.view_month);
	if (mc_clock_calendar.selected_day < 1)
	{
		mc_clock_calendar.selected_day = 1;
	}
	if (mc_clock_calendar.selected_day > max_day)
	{
		mc_clock_calendar.selected_day = max_day;
	}
}

/* The list view shows only the selected day's events, CALENDAR_LIST_VISIBLE at a
 * time, and scroll_offset counts positions within that day. Navigation below
 * walks the same filtered list so the cursor never lands on a hidden event. */

/* Copy of the event being edited, restored when an edit is abandoned */
static mc_clock_calendar_event_t calendar_edit_backup;

static uint8_t mc_clock_calendar_on_selected_day(uint8_t idx)
{
	const mc_clock_calendar_event_t* ev = &mc_clock_calendar.events[idx];

	return ev->year == mc_clock_calendar.view_year &&
	       ev->month == mc_clock_calendar.view_month &&
	       ev->day == mc_clock_calendar.selected_day;
}

/* Next (dir > 0) or previous (dir < 0) selected-day event from idx, or CALENDAR_EVENT_NONE */
static uint8_t mc_clock_calendar_find_on_day(int16_t idx, int8_t dir)
{
	for (idx += dir; idx >= 0 && idx < mc_clock_calendar.total_events; idx += dir)
	{
		if (mc_clock_calendar_on_selected_day((uint8_t)idx))
		{
			return (uint8_t)idx;
		}
	}

	return CALENDAR_EVENT_NONE;
}

/* Helper - scroll event list to keep current_event visible                  */
static void mc_clock_calendar_scroll_to_event(void)
{
	if (mc_clock_calendar.current_event >= mc_clock_calendar.total_events)
	{
		mc_clock_calendar.scroll_offset = 0;
		return;
	}

	uint8_t pos = 0;

	for (uint8_t i = 0; i < mc_clock_calendar.current_event; i++)
	{
		if (mc_clock_calendar_on_selected_day(i))
		{
			pos++;
		}
	}

	if (pos < mc_clock_calendar.scroll_offset)
	{
		mc_clock_calendar.scroll_offset = pos;
	}
	else if (pos >= mc_clock_calendar.scroll_offset + CALENDAR_LIST_VISIBLE)
	{
		mc_clock_calendar.scroll_offset = pos - (CALENDAR_LIST_VISIBLE - 1);
	}
}

/* Move the month view by one month; the caller picks the selected day */
static void mc_clock_calendar_shift_month(int8_t dir)
{
	if (dir > 0)
	{
		if (mc_clock_calendar.view_month == 12)
		{
			mc_clock_calendar.view_month = 1;
			mc_clock_calendar.view_year++;
		}
		else
		{
			mc_clock_calendar.view_month++;
		}
	}
	else
	{
		if (mc_clock_calendar.view_month == 1)
		{
			mc_clock_calendar.view_month = 12;
			mc_clock_calendar.view_year--;
		}
		else
		{
			mc_clock_calendar.view_month--;
		}
	}
}

/* Helper - increment / decrement a field value                              */
static void mc_clock_calendar_field_change(int8_t delta)
{
	if (mc_clock_calendar.editing_event >= mc_clock_calendar.total_events)
	{
		return;
	}

	mc_clock_calendar_event_t* ev = &mc_clock_calendar.events[mc_clock_calendar.editing_event];
	uint8_t days;

	switch (mc_clock_calendar.editing_field)
	{
	case CALENDAR_FIELD_YEAR:
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
		days = mc_clock_date_days_in_month(ev->year, ev->month);
		if (ev->day > days)
			ev->day = days;
		break;

	case CALENDAR_FIELD_MONTH:
		if (delta > 0)
		{
			ev->month = (ev->month % 12) + 1;
		}
		else
		{
			ev->month = (ev->month == 1) ? 12 : ev->month - 1;
		}
		days = mc_clock_date_days_in_month(ev->year, ev->month);
		if (ev->day > days)
			ev->day = days;
		break;

	case CALENDAR_FIELD_DAY:
		days = mc_clock_date_days_in_month(ev->year, ev->month);
		if (delta > 0)
		{
			ev->day = (ev->day % days) + 1;
		}
		else
		{
			ev->day = (ev->day == 1) ? days : ev->day - 1;
		}
		break;

	case CALENDAR_FIELD_CATEGORY:
		if (delta > 0)
		{
			ev->category = (ev->category + 1) % CALENDAR_CATEGORY_MAX;
		}
		else
		{
			ev->category = (ev->category == 0)
			                   ? (CALENDAR_CATEGORY_MAX - 1)
			                   : (ev->category - 1);
		}
		break;

	case CALENDAR_FIELD_START_HOUR:
		if (delta > 0)
		{
			ev->start_hour = (ev->start_hour + 1) % 24;
		}
		else
		{
			ev->start_hour = (ev->start_hour == 0) ? 23 : ev->start_hour - 1;
		}
		break;

	case CALENDAR_FIELD_START_MIN:
		if (delta > 0)
		{
			ev->start_min = (ev->start_min + 1) % 60;
		}
		else
		{
			ev->start_min = (ev->start_min == 0) ? 59 : ev->start_min - 1;
		}
		break;

	case CALENDAR_FIELD_END_HOUR:
		if (delta > 0)
		{
			ev->end_hour = (ev->end_hour + 1) % 24;
		}
		else
		{
			ev->end_hour = (ev->end_hour == 0) ? 23 : ev->end_hour - 1;
		}
		break;

	case CALENDAR_FIELD_END_MIN:
		if (delta > 0)
		{
			ev->end_min = (ev->end_min + 1) % 60;
		}
		else
		{
			ev->end_min = (ev->end_min == 0) ? 59 : ev->end_min - 1;
		}
		break;

	case CALENDAR_FIELD_ALARM:
		ev->alarm_enabled = !ev->alarm_enabled;
		break;

	default:
		break;
	}
}

void mc_clock_calendar_init(void)
{
	memset(&mc_clock_calendar, 0, sizeof(mc_clock_calendar));

	rtc_date_t date;
	rtc_get_date(&date);

	mc_clock_calendar.today_year = date.year;
	mc_clock_calendar.today_month = date.month;
	mc_clock_calendar.today_day = date.date;

	mc_clock_calendar.view_year = date.year;
	mc_clock_calendar.view_month = date.month;
	mc_clock_calendar.selected_day = date.date;

	mc_clock_calendar.total_events = 0;
	mc_clock_calendar.mode = CALENDAR_MODE_MONTH;
}

void mc_clock_calendar_save_to_eeprom(void)
{
	mc_clock_calendar_eeprom_t eeprom_data;

	eeprom_data.data.total_events = mc_clock_calendar.total_events;
	for (uint8_t i = 0; i < CALENDAR_EVENT_NUMBER; i++)
	{
		eeprom_data.data.events[i] = mc_clock_calendar.events[i];
	}

	mc_clock_eeprom_update_checksum(&eeprom_data.magic_number,
	                                &eeprom_data.check_sum,
	                                CALENDAR_CHECKSUM_SIZE);

	eeprom_write(EEPROM_CALENDAR_START_ADDR,
	             (uint8_t*)&eeprom_data,
	             sizeof(eeprom_data));
}

void mc_clock_calendar_load_from_eeprom(void)
{
	mc_clock_calendar_eeprom_t eeprom_data;

	uint8_t ret = eeprom_read(EEPROM_CALENDAR_START_ADDR,
	                          (uint8_t*)&eeprom_data,
	                          sizeof(eeprom_data));

	if (ret == EEPROM_DRIVER_OK &&
	    mc_clock_eeprom_is_valid(&eeprom_data.magic_number,
	                             eeprom_data.check_sum,
	                             CALENDAR_CHECKSUM_SIZE))
	{
		mc_clock_calendar.total_events = eeprom_data.data.total_events;
		if (mc_clock_calendar.total_events > CALENDAR_EVENT_NUMBER)
		{
			mc_clock_calendar.total_events = 0;
		}
		for (uint8_t i = 0; i < CALENDAR_EVENT_NUMBER; i++)
		{
			mc_clock_calendar.events[i] = eeprom_data.data.events[i];
		}
	}
}

void mc_clock_calendar_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_CALENDAR_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_SETUP\n");

		mc_clock_calendar_sync_today();

		if (mc_clock_calendar.view_year == 0)
		{
			mc_clock_calendar.view_year = mc_clock_calendar.today_year;
			mc_clock_calendar.view_month = mc_clock_calendar.today_month;
			mc_clock_calendar.selected_day = mc_clock_calendar.today_day;
		}

		mc_clock_calendar.mode = CALENDAR_MODE_MONTH;
		mc_clock_calendar.current_event = CALENDAR_EVENT_NONE;
		mc_clock_calendar.editing_event = CALENDAR_EVENT_NONE;
		mc_clock_calendar.editing_field = CALENDAR_FIELD_YEAR;
		mc_clock_calendar.scroll_offset = 0;
		mc_clock_calendar.is_new_event = 0;
		mc_clock_calendar.ringing = 0;
	}
	break;

	/* 1-second tick: update today's date and check event alarms           */
	case MC_CLOCK_CALENDAR_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_UPDATE\n");
		mc_clock_calendar_sync_today();

		if (mc_clock_calendar.ringing)
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
		for (uint8_t i = 0; i < mc_clock_calendar.total_events; i++)
		{
			mc_clock_calendar_event_t* ev = &mc_clock_calendar.events[i];

			uint8_t due = ev->alarm_enabled &&
			              ev->year == mc_clock_calendar.today_year &&
			              ev->month == mc_clock_calendar.today_month &&
			              ev->day == mc_clock_calendar.today_day &&
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
				mc_clock_calendar.ringing = 1;
				mc_clock_calendar.ringing_event = i;
				task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_ALARM_FIRED);
				break;
			}
		}
	}
	break;

	case MC_CLOCK_CALENDAR_ALARM_FIRED:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_ALARM_FIRED\n");
		mc_clock_calendar.ringing = 1;
		BUZZER_PlaySound((buzzer_sound_t)mc_clock_alarm_get_sound());
		mc_clock_ring_request(RING_SOURCE_CALENDAR, mc_clock_alarm_get_sound());
	}
	break;

	case MC_CLOCK_CALENDAR_ALARM_DISMISS:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_ALARM_DISMISS\n");
		mc_clock_calendar.ringing = 0;
		BUZZER_Disable();
	}
	break;

	case MC_CLOCK_CALENDAR_NEXT_MONTH:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_NEXT_MONTH\n");
		mc_clock_calendar_shift_month(+1);
		mc_clock_calendar_clamp_day();
	}
	break;

	case MC_CLOCK_CALENDAR_PREV_MONTH:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_PREV_MONTH\n");
		mc_clock_calendar_shift_month(-1);
		mc_clock_calendar_clamp_day();
	}
	break;

	/* Add a new event pre-filled with the currently selected date         */
	case MC_CLOCK_CALENDAR_ADD_EVENT:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_ADD_EVENT\n");

		if (mc_clock_calendar.total_events >= CALENDAR_EVENT_NUMBER)
		{
			break;
		}

		uint8_t idx = mc_clock_calendar.total_events;
		memset(&mc_clock_calendar.events[idx], 0, sizeof(mc_clock_calendar_event_t));

		mc_clock_calendar.events[idx].year = mc_clock_calendar.view_year;
		mc_clock_calendar.events[idx].month = mc_clock_calendar.view_month;
		mc_clock_calendar.events[idx].day = mc_clock_calendar.selected_day;
		mc_clock_calendar.events[idx].start_hour = CALENDAR_NEW_START_HOUR;
		mc_clock_calendar.events[idx].start_min = 0;
		mc_clock_calendar.events[idx].end_hour = CALENDAR_NEW_END_HOUR;
		mc_clock_calendar.events[idx].end_min = 0;
		mc_clock_calendar.events[idx].category = CALENDAR_CATEGORY_WORK;
		mc_clock_calendar.events[idx].alarm_enabled = 1;
		mc_clock_calendar.events[idx].alarm_fired = 0;

		mc_clock_calendar.total_events++;
		mc_clock_calendar.editing_event = idx;
		mc_clock_calendar.editing_field = CALENDAR_FIELD_YEAR;
		mc_clock_calendar.is_new_event = 1;
		mc_clock_calendar.mode = CALENDAR_MODE_EDIT;
	}
	break;

	/* From MONTH view: switch to event LIST for selected day.             */
	/* From LIST view: enter EDIT mode for the selected event.             */
	case MC_CLOCK_CALENDAR_EDIT_EVENT:
	{
		APP_DBG_SIG("EDIT EVENT MODE=%u CURRENT=%u TOTAL=%u\n",
		            mc_clock_calendar.mode,
		            mc_clock_calendar.current_event,
		            mc_clock_calendar.total_events);

		if (mc_clock_calendar.mode == CALENDAR_MODE_MONTH)
		{
			mc_clock_calendar.current_event = mc_clock_calendar_find_on_day(-1, +1);
			mc_clock_calendar.scroll_offset = 0;
			mc_clock_calendar.mode = CALENDAR_MODE_LIST;
		}
		else if (mc_clock_calendar.mode == CALENDAR_MODE_LIST)
		{
			if (mc_clock_calendar.current_event >= mc_clock_calendar.total_events)
			{
				break;
			}

			mc_clock_calendar.editing_event = mc_clock_calendar.current_event;
			mc_clock_calendar.editing_field = CALENDAR_FIELD_YEAR;
			mc_clock_calendar.is_new_event = 0;
			calendar_edit_backup = mc_clock_calendar.events[mc_clock_calendar.editing_event];

			mc_clock_calendar.mode = CALENDAR_MODE_EDIT;
		}
	}
	break;

	case MC_CLOCK_CALENDAR_DELETE_EVENT:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_DELETE_EVENT\n");

		uint8_t del;

		if (mc_clock_calendar.mode == CALENDAR_MODE_EDIT)
			del = mc_clock_calendar.editing_event;
		else
			del = mc_clock_calendar.current_event;

		if (del < mc_clock_calendar.total_events)
		{
			for (uint8_t i = del; i + 1 < mc_clock_calendar.total_events; i++)
			{
				mc_clock_calendar.events[i] = mc_clock_calendar.events[i + 1];
			}

			mc_clock_calendar.total_events--;

			/* Select the next event of the same day (it slid into del), else
			 * the previous one; with none left, go back to the month view. */
			mc_clock_calendar.current_event = mc_clock_calendar_find_on_day((int16_t)del - 1, +1);
			if (mc_clock_calendar.current_event == CALENDAR_EVENT_NONE)
			{
				mc_clock_calendar.current_event = mc_clock_calendar_find_on_day(del, -1);
			}

			mc_clock_calendar.is_new_event = 0;

			if (mc_clock_calendar.current_event == CALENDAR_EVENT_NONE)
			{
				mc_clock_calendar.mode = CALENDAR_MODE_MONTH;
			}
			else
			{
				mc_clock_calendar.mode = CALENDAR_MODE_LIST;
			}

			mc_clock_calendar_scroll_to_event();
			mc_clock_calendar_save_to_eeprom();
		}
	}
	break;

	case MC_CLOCK_CALENDAR_NEXT_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_NEXT_FIELD\n");

		if (mc_clock_calendar.mode != CALENDAR_MODE_EDIT)
		{
			break;
		}

		if (mc_clock_calendar.editing_field < (CALENDAR_FIELD_MAX - 1))
		{
			/* Move to next editable field */
			mc_clock_calendar.editing_field++;
		}
		else
		{
			/* Finished editing */

			mc_clock_calendar_event_t* ev = &mc_clock_calendar.events[mc_clock_calendar.editing_event];
			ev->alarm_fired = 0;

			mc_clock_calendar.view_year = ev->year;
			mc_clock_calendar.view_month = ev->month;
			mc_clock_calendar.selected_day = ev->day;

			mc_clock_calendar.current_event = mc_clock_calendar.editing_event;
			mc_clock_calendar.scroll_offset = 0;
			mc_clock_calendar_scroll_to_event();

			if (mc_clock_calendar.is_new_event)
			{
				mc_clock_calendar.is_new_event = 0;
				mc_clock_calendar.mode = CALENDAR_MODE_MONTH;
			}
			else
			{
				mc_clock_calendar.mode = CALENDAR_MODE_LIST;
			}

			mc_clock_calendar_save_to_eeprom();
		}
	}
	break;

	case MC_CLOCK_CALENDAR_FIELD_INC:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_FIELD_INC\n");

		if (mc_clock_calendar.mode == CALENDAR_MODE_EDIT)
		{
			mc_clock_calendar_field_change(+1);
		}
		else if (mc_clock_calendar.mode == CALENDAR_MODE_MONTH)
		{
			/* In month view UP increments selected day */
			uint8_t max_day = mc_clock_date_days_in_month(mc_clock_calendar.view_year, mc_clock_calendar.view_month);
			if (mc_clock_calendar.selected_day < max_day)
			{
				mc_clock_calendar.selected_day++;
			}
			else
			{
				mc_clock_calendar_shift_month(+1);
				mc_clock_calendar.selected_day = 1;
			}
		}
		else if (mc_clock_calendar.mode == CALENDAR_MODE_LIST)
		{
			uint8_t next = mc_clock_calendar_find_on_day(mc_clock_calendar.current_event, +1);
			if (mc_clock_calendar.current_event < mc_clock_calendar.total_events && next != CALENDAR_EVENT_NONE)
			{
				mc_clock_calendar.current_event = next;
				mc_clock_calendar_scroll_to_event();
			}
		}
	}
	break;

	/* ------------------------------------------------------------------ */
	case MC_CLOCK_CALENDAR_FIELD_DEC:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_FIELD_DEC\n");

		if (mc_clock_calendar.mode == CALENDAR_MODE_EDIT)
		{
			mc_clock_calendar_field_change(-1);
		}
		else if (mc_clock_calendar.mode == CALENDAR_MODE_MONTH)
		{
			if (mc_clock_calendar.selected_day > 1)
			{
				mc_clock_calendar.selected_day--;
			}
			else
			{
				/* Last day of the previous month */
				mc_clock_calendar_shift_month(-1);
				mc_clock_calendar.selected_day =
				    mc_clock_date_days_in_month(mc_clock_calendar.view_year, mc_clock_calendar.view_month);
			}
		}
		else if (mc_clock_calendar.mode == CALENDAR_MODE_LIST)
		{
			if (mc_clock_calendar.current_event >= mc_clock_calendar.total_events)
			{
				/* no event on this day -> create new event */
				task_post_pure_msg(
				    MC_CLOCK_CALENDAR_ID,
				    MC_CLOCK_CALENDAR_ADD_EVENT);

				break;
			}

			uint8_t prev = mc_clock_calendar_find_on_day(mc_clock_calendar.current_event, -1);
			if (prev != CALENDAR_EVENT_NONE)
			{
				mc_clock_calendar.current_event = prev;
				mc_clock_calendar_scroll_to_event();
			}
		}
	}
	break;

	case MC_CLOCK_CALENDAR_BACK:
	{
		APP_DBG_SIG("MC_CLOCK_CALENDAR_BACK\n");

		if (mc_clock_calendar.mode == CALENDAR_MODE_LIST)
		{
			mc_clock_calendar.mode = CALENDAR_MODE_MONTH;
		}
		else if (mc_clock_calendar.mode == CALENDAR_MODE_EDIT)
		{
			if (mc_clock_calendar.is_new_event)
			{
				/* Abandoned new event: drop it (it was never saved) */
				mc_clock_calendar.total_events--;
				mc_clock_calendar.is_new_event = 0;
				mc_clock_calendar.current_event = mc_clock_calendar_find_on_day(-1, +1);
			}
			else if (mc_clock_calendar.editing_event < mc_clock_calendar.total_events)
			{
				mc_clock_calendar.events[mc_clock_calendar.editing_event] = calendar_edit_backup;
			}

			mc_clock_calendar.mode = CALENDAR_MODE_LIST;
			mc_clock_calendar_scroll_to_event();
		}
	}
	break;

	/* ------------------------------------------------------------------ */
	default:
		break;
	}

	/* Button-driven changes: the display rendered the press before this handler
	 * ran, so redraw now instead of on the next 1 s screen tick. The background
	 * tick (UPDATE) and the ringing path do not need a redraw here. */
	if (msg->sig != MC_CLOCK_CALENDAR_UPDATE &&
	    msg->sig != MC_CLOCK_CALENDAR_ALARM_FIRED &&
	    msg->sig != MC_CLOCK_CALENDAR_ALARM_DISMISS)
	{
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
}

void mc_clock_calendar_sync(void)
{
	mc_clock_calendar_sync_today();
}