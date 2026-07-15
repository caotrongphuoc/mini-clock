#include "scr_clock_calender.h"

#include "mc_clock_calender.h"
#include "task_list.h"
#include "timer.h"

/*****************************************************************************/
/* Month names                                                                */
/*****************************************************************************/

static const char* const MONTH_NAME[13] = {
    "",
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};

/*****************************************************************************/
/* Day names                                                                */
/*****************************************************************************/

static const char* const DOW_NAME[7] = {
    "Mo",
    "Tu",
    "We",
    "Th",
    "Fr",
    "Sa",
    "Su",
};

/*****************************************************************************/
/* View helper — weekday of 1st of month (0=Mon … 6=Sun)           			 */
/*****************************************************************************/

static uint8_t first_weekday_of_month(uint8_t month, uint16_t year)
{
	/* Tomohiko Sakamoto's algorithm, returns 0=Sun … 6=Sat              */
	/* We remap to 0=Mon … 6=Sun                                         */
	static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	if (month < 3)
		year--;
	int dow = (year + year / 4 - year / 100 + year / 400 + t[month - 1] + 1) % 7;
	/* 0=Sun → convert to 0=Mon: Sun becomes 6, others shift left */
	return (uint8_t)((dow + 6) % 7);
}

/*****************************************************************************/
/* View — Monthly grid                                                        */
/*****************************************************************************/

static void view_scr_calendar_month(const mc_calendar_state_t* st)
{
	char buf[20];
	uint8_t first_dow = first_weekday_of_month(st->view_month, st->view_year);
	uint8_t days_in_m = mc_calendar_days_in_month(st->view_month, st->view_year);
	/* Scroll so the selected week stays visible */
	uint8_t selected_cell = first_dow + (st->selected_day - 1);
	uint8_t selected_row = selected_cell / 7;

	int16_t scroll_y = 0;

	if (selected_row >= 3)
	{
		scroll_y = (selected_row - 2) * 9;
	}

	/* Header: month name + year */
	xsprintf(buf, "%s %u", MONTH_NAME[st->view_month], st->view_year);
	view_render.setTextSize(1);
	view_render.setCursor(2, 2);
	view_render.print(buf);

	/* Navigation hint */
	view_render.setCursor(82, 2);
	view_render.print("< MON >");

	/* Day-of-week headers */
	for (uint8_t d = 0; d < 7; d++)
	{
		view_render.setCursor(2 + d * 18, 12);
		view_render.print(DOW_NAME[d]);
	}

	view_render.drawLine(0, 20, 128, 20, WHITE);

	for (uint8_t day = 1; day <= days_in_m; day++)
	{
		uint8_t cell_idx = first_dow + (day - 1);
		uint8_t col = cell_idx % 7;
		uint8_t row = cell_idx / 7;

		int16_t x = 2 + col * 18;
		int16_t y = 22 + row * 9 - scroll_y;

		if (y < 21 || y > 63)
		{
			continue;
		}

		uint8_t is_today = (day == st->today_day &&
		                    st->view_month == st->today_month &&
		                    st->view_year == st->today_year);
		uint8_t is_selected = (day == st->selected_day);
		uint8_t has_event = mc_calendar_has_event_on_day(
		    st->view_year, st->view_month, day);

		if (is_selected)
		{
			view_render.fillRect(x - 1, y - 1, 14, 9, WHITE);
			view_render.setTextColor(BLACK);
		}
		else
		{
			view_render.setTextColor(WHITE);
		}

		if (is_today && !is_selected)
		{
			view_render.drawRect(x - 1, y - 1, 14, 9, WHITE);
		}

		xsprintf(buf, "%2u", day);
		view_render.setCursor(x, y);
		view_render.print(buf);

		view_render.setTextColor(WHITE);

		/* Event dot */
		if (has_event)
		{
			view_render.fillRect(x + 10, y + 6, 2, 2,
			                     is_selected ? BLACK : WHITE);
		}
	}
}

/*****************************************************************************/
/* View — Event list for selected day                                         */
/*****************************************************************************/

static uint8_t count_events_on_day(const mc_calendar_state_t* st)
{
	uint8_t count = 0;
	for (uint8_t i = 0; i < st->total_events; i++)
	{
		if (st->events[i].year == st->view_year &&
		    st->events[i].month == st->view_month &&
		    st->events[i].day == st->selected_day)
		{
			count++;
		}
	}
	return count;
}

static void view_scr_calendar_list(const mc_calendar_state_t* st)
{
	char buf[24];

	/* Header */
	xsprintf(buf, "%u %s %u", st->selected_day,
	         MONTH_NAME[st->view_month], st->view_year);
	view_render.setTextSize(1);
	view_render.setCursor(2, 2);
	view_render.print(buf);

	view_render.drawLine(0, 10, 128, 10, WHITE);

	/* Events for this day (up to 3 visible) */
	uint8_t shown = 0;
	uint8_t ev_idx = 0; /* global events[] index for items shown */

	for (uint8_t i = 0; i < st->total_events; i++)
	{
		const mc_calendar_event_t* ev = &st->events[i];

		if (ev->year != st->view_year ||
		    ev->month != st->view_month ||
		    ev->day != st->selected_day)
		{
			continue;
		}

		if (ev_idx < st->scroll_offset)
		{
			ev_idx++;
			continue;
		}

		if (shown >= 3)
		{
			break;
		}

		int16_t y = 13 + shown * 15;
		uint8_t sel = (i == st->current_event);
		uint16_t bg = sel ? WHITE : BLACK;
		uint16_t fg = sel ? BLACK : WHITE;

		view_render.fillRect(0, y, 128, 14, bg);
		view_render.setTextColor(fg);
		view_render.setTextSize(1);

		/* Category tag */
		view_render.setCursor(2, y + 2);
		view_render.print(MC_CAL_CAT_NAME[ev->category]);

		/* Time */
		xsprintf(buf, "%02u:%02u-%02u:%02u",
		         ev->start_hour, ev->start_min,
		         ev->end_hour, ev->end_min);
		view_render.setCursor(52, y + 2);
		view_render.print(buf);

		/* Alarm indicator */
		view_render.setCursor(114, y + 2);
		view_render.print(ev->alarm_enabled ? "A" : " ");

		view_render.setTextColor(WHITE);

		shown++;
		ev_idx++;
	}

	if (shown == 0)
	{
		view_render.setCursor(20, 28);
		view_render.print("No events today");
	}

	/* Bottom hints */
	view_render.drawLine(0, 50, 128, 50, WHITE);
	view_render.setCursor(2, 52);
	view_render.print("Edit    Del    Back");
}

/*****************************************************************************/
/* View — Edit event                                                          */
/*****************************************************************************/

static const char* const FIELD_LABEL[MC_CAL_FIELD_MAX] = {
    "Year",
    "Month",
    "Day",
    "Category",
    "Start H",
    "Start M",
    "End H",
    "End M",
    "Alarm",
};

static void view_scr_calendar_edit(const mc_calendar_state_t* st)
{
	if (st->editing_event >= st->total_events)
	{
		return;
	}

	const mc_calendar_event_t* ev = &st->events[st->editing_event];
	char buf[20];

	view_render.drawRoundRect(0, 0, 128, 64, 4, WHITE);

	/* Title */
	view_render.setTextSize(1);
	view_render.setCursor(4, 4);
	view_render.print(st->is_new_event ? "Add Event" : "Edit Event");

	/* Field label */
	view_render.setCursor(4, 16);
	view_render.print(FIELD_LABEL[st->editing_field]);

	/* Current value */
	view_render.setTextSize(2);
	view_render.setCursor(4, 28);

	switch (st->editing_field)
	{
	case MC_CAL_FIELD_YEAR:
		xsprintf(buf, "%u", ev->year);
		break;
	case MC_CAL_FIELD_MONTH:
		xsprintf(buf, "%s", MONTH_NAME[ev->month]);
		break;
	case MC_CAL_FIELD_DAY:
		xsprintf(buf, "%u", ev->day);
		break;
	case MC_CAL_FIELD_CATEGORY:
		xsprintf(buf, "%s", MC_CAL_CAT_NAME[ev->category]);
		break;
	case MC_CAL_FIELD_START_HOUR:
	case MC_CAL_FIELD_END_HOUR:
		xsprintf(buf, "%02u", (st->editing_field == MC_CAL_FIELD_START_HOUR) ? ev->start_hour : ev->end_hour);
		break;
	case MC_CAL_FIELD_START_MIN:
	case MC_CAL_FIELD_END_MIN:
		xsprintf(buf, "%02u", (st->editing_field == MC_CAL_FIELD_START_MIN) ? ev->start_min : ev->end_min);
		break;
	case MC_CAL_FIELD_ALARM:
		xsprintf(buf, "%s", ev->alarm_enabled ? "ON" : "OFF");
		break;
	default:
		buf[0] = '\0';
		break;
	}

	view_render.print(buf);
	view_render.setTextSize(1);

	/* Progress bar */
	uint8_t bar_w = (uint8_t)(((uint16_t)(st->editing_field + 1) * 110) / MC_CAL_FIELD_MAX);
	view_render.fillRect(8, 54, bar_w, 4, WHITE);
	// view_render.drawRect(8, 54, 110, 4, WHITE);

	/* Hint */
	view_render.setCursor(4, 59);
	view_render.print("UP/DN:val  MD:next");
}

/*****************************************************************************/
/* View — Ringing overlay                                                     */
/*****************************************************************************/

static void view_scr_calendar_ringing(const mc_calendar_state_t* st)
{
	char buf[20];

	view_render.drawRoundRect(8, 8, 112, 48, 5, WHITE);
	view_render.fillRect(10, 10, 108, 44, BLACK);
	view_render.drawRoundRect(10, 10, 108, 44, 4, WHITE);

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	view_render.setCursor(32, 14);
	view_render.print("* REMINDER *");

	if (st->ringing_event < st->total_events)
	{
		const mc_calendar_event_t* ev = &st->events[st->ringing_event];

		view_render.setCursor(14, 28);
		view_render.print(MC_CAL_CAT_NAME[ev->category]);

		xsprintf(buf, "%02u:%02u", ev->start_hour, ev->start_min);
		view_render.setCursor(80, 28);
		view_render.print(buf);
	}

	view_render.setCursor(22, 44);
	view_render.print("MODE: dismiss");
}

/*****************************************************************************/
/* Main view dispatch                                                         */
/*****************************************************************************/

static void view_scr_clock_calender();

view_dynamic_t dyn_view_scr_clock_calender = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_calender};

view_screen_t scr_clock_calender = {
    &dyn_view_scr_clock_calender,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

static void view_scr_clock_calender()
{
	mc_calendar_state_t st;
	mc_clock_calendar_get_state(&st);

	view_render.clear();
	view_render.setTextColor(WHITE);
	// view_render.drawRect(0, 0, 128, 64, WHITE);

	if (st.ringing)
	{
		view_scr_calendar_ringing(&st);
		return;
	}

	switch (st.mode)
	{
	case MC_CAL_MODE_MONTH:
		view_scr_calendar_month(&st);
		break;

	case MC_CAL_MODE_LIST:
		view_scr_calendar_list(&st);
		break;

	case MC_CAL_MODE_EDIT:
		view_scr_calendar_edit(&st);
		break;

	default:
		view_scr_calendar_month(&st);
		break;
	}
}

void scr_clock_calender_handle(ak_msg_t* msg)
{
	mc_calendar_state_t st;

	switch (msg->sig)
	{

	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY [calendar]\n");
		mc_clock_calendar_sync();
		timer_set(AC_TASK_DISPLAY_ID,
		          MC_CLOCK_TIME_TICK,
		          MC_CLOCK_TIME_TICK_INTERVAL,
		          TIMER_PERIODIC);
	}
	break;

	case MC_CLOCK_TIME_TICK:
	{
		/* Every second: check alarms */
		task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED [calendar]\n");
		mc_clock_calendar_get_state(&st);

		if (st.ringing)
		{
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_ALARM_DISMISS);
			BUZZER_Disable();
			break;
		}

		switch (st.mode)
		{
		case MC_CAL_MODE_MONTH:
			/* MODE in month view: go to event list for selected day */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_EDIT_EVENT);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case MC_CAL_MODE_LIST:
			/* MODE in list view: edit selected event */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_EDIT_EVENT);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case MC_CAL_MODE_EDIT:
			/* MODE in edit view: advance to next field / confirm */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_NEXT_FIELD);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		default:
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_MODE_PRESSED [calendar]\n");
		mc_clock_calendar_get_state(&st);

		if (st.ringing)
		{
			break;
		}

		switch (st.mode)
		{
		case MC_CAL_MODE_MONTH:
		{
			/* Add new event */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
			                   MC_CLOCK_CALENDAR_ADD_EVENT);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		case MC_CAL_MODE_LIST:
		{
			/* Delete selected event */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
			                   MC_CLOCK_CALENDAR_DELETE_EVENT);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		case MC_CAL_MODE_EDIT:
		{
			if (st.is_new_event)
			{
				/* Cancel new event */
				task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
				                   MC_CLOCK_CALENDAR_DELETE_EVENT);
			}
			else
			{
				/* Finish editing */
				task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
				                   MC_CLOCK_CALENDAR_NEXT_FIELD);

				/* Jump directly to last field so NEXT_FIELD exits edit mode */
				while (st.editing_field < (MC_CAL_FIELD_MAX - 1))
				{
					task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
					                   MC_CLOCK_CALENDAR_NEXT_FIELD);
					st.editing_field++;
				}

				task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
				                   MC_CLOCK_CALENDAR_NEXT_FIELD);
			}

			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		default:
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED [calendar]\n");
		mc_clock_calendar_get_state(&st);

		if (st.ringing)
		{
			break;
		}

		if (st.mode == MC_CAL_MODE_MONTH)
		{
			/* UP = previous day (month view pans left) */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_FIELD_DEC);
		}
		else
		{
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_FIELD_INC);
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED [calendar]\n");
		mc_clock_calendar_get_state(&st);

		if (st.ringing)
		{
			break;
		}

		if (st.mode == MC_CAL_MODE_MONTH)
		{
			/* DOWN = next day */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_FIELD_INC);
		}
		else
		{
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_FIELD_DEC);
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_UP_PRESSED [calendar]\n");
		mc_clock_calendar_get_state(&st);

		if (st.mode == MC_CAL_MODE_MONTH)
		{
			/* Long-UP: previous month */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_PREV_MONTH);
		}
		else if (st.mode == MC_CAL_MODE_LIST)
		{
			/* Long-UP in list: back to month view */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_SETUP);
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_DOWN_PRESSED [calendar]\n");
		mc_clock_calendar_get_state(&st);

		if (st.mode == MC_CAL_MODE_MONTH)
		{
			/* Long-DOWN: next month */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_NEXT_MONTH);
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}