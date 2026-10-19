#include "scr_clock_calendar.h"
#include "mc_clock_date.h"

#include "mc_clock_calendar.h"
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
/* View — Monthly grid                                                        */
/*****************************************************************************/

/*
 * Grid geometry: 7 columns × 18 px = 126 px (1 px margin each side)
 * Rows: 9 px tall, up to 5 visible at once (scroll for month-6)
 */
#define CALENDAR_COL_W (18)
#define CALENDAR_COL_CENTER(c) (10 + (c)*CALENDAR_COL_W)
#define CALENDAR_ROW_H (9)
#define CALENDAR_GRID_Y (19)

static void view_scr_calendar_month()
{
	char buf[16];
	/* Grid column of the 1st: 0 = Monday ... 6 = Sunday */
	uint8_t first_dow = mc_clock_date_weekday(mc_clock_calendar.view_year, mc_clock_calendar.view_month, 1) - RTC_WEEKDAY_MON;
	uint8_t days_in_m = mc_clock_date_days_in_month(mc_clock_calendar.view_year, mc_clock_calendar.view_month);
	uint8_t selected_cell = first_dow + (mc_clock_calendar.selected_day - 1);
	uint8_t selected_row = selected_cell / 7;

	int16_t scroll_y = 0;
	if (selected_row >= 4)
	{
		scroll_y = (selected_row - 3) * CALENDAR_ROW_H;
	}

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	/* Header "Mmm YYYY" - always 8 chars = 47 px, centered */
	xsprintf(buf, "%s %u", MONTH_NAME[mc_clock_calendar.view_month], mc_clock_calendar.view_year);
	view_render.setCursor(40, 1);
	view_render.print(buf);

	/* DOW headers centered per column */
	for (uint8_t d = 0; d < 7; d++)
	{
		view_render.setCursor(CALENDAR_COL_CENTER(d) - 5, 9);
		view_render.print(DOW_NAME[d]);
	}

	view_render.drawLine(0, 17, LCD_WIDTH - 1, 17, WHITE);

	for (uint8_t day = 1; day <= days_in_m; day++)
	{
		uint8_t cell_idx = first_dow + (day - 1);
		uint8_t col = cell_idx % 7;
		uint8_t row = cell_idx / 7;

		int16_t center_x = CALENDAR_COL_CENTER(col);
		int16_t y = CALENDAR_GRID_Y + row * CALENDAR_ROW_H - scroll_y;

		if (y < 18 || y > 61)
		{
			continue;
		}

		uint8_t is_today = (day == mc_clock_calendar.today_day &&
		                    mc_clock_calendar.view_month == mc_clock_calendar.today_month &&
		                    mc_clock_calendar.view_year == mc_clock_calendar.today_year);
		uint8_t is_selected = (day == mc_clock_calendar.selected_day);
		uint8_t has_event = mc_clock_calendar_has_event_on_day(
		    mc_clock_calendar.view_year, mc_clock_calendar.view_month, day);

		int16_t box_x = center_x - 6;

		if (is_selected)
		{
			view_render.fillRect(box_x, y - 1, 13, 8, WHITE);
			view_render.setTextColor(BLACK);
		}
		else
		{
			view_render.setTextColor(WHITE);
			if (is_today)
			{
				view_render.drawRect(box_x, y - 1, 13, 8, WHITE);
			}
		}

		xsprintf(buf, "%2u", day);
		view_render.setCursor(center_x - 5, y);
		view_render.print(buf);

		view_render.setTextColor(WHITE);

		if (has_event)
		{
			view_render.fillRect(center_x - 1, y + 8, 2, 1, WHITE);
		}
	}
}

/*****************************************************************************/
/* View — Event list for selected day                                         */
/*****************************************************************************/

static void view_scr_calendar_list()
{
	char buf[24];

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	/* Header */
	xsprintf(buf, "%u %s %u", mc_clock_calendar.selected_day,
	         MONTH_NAME[mc_clock_calendar.view_month], mc_clock_calendar.view_year);
	view_render.setCursor(2, 1);
	view_render.print(buf);

	view_render.drawLine(0, 9, LCD_WIDTH - 1, 9, WHITE);

	/* Events, 2-line layout, up to 2 visible (17 tall each) */
	uint8_t shown = 0;
	uint8_t ev_idx = 0;

	for (uint8_t i = 0; i < mc_clock_calendar.total_events; i++)
	{
		const mc_clock_calendar_event_t* ev = &mc_clock_calendar.events[i];

		if (ev->year != mc_clock_calendar.view_year ||
		    ev->month != mc_clock_calendar.view_month ||
		    ev->day != mc_clock_calendar.selected_day)
		{
			continue;
		}

		if (ev_idx < mc_clock_calendar.scroll_offset)
		{
			ev_idx++;
			continue;
		}

		if (shown >= 2)
		{
			break;
		}

		int16_t y = 11 + shown * 18;
		uint8_t sel = (mc_clock_calendar.current_event < mc_clock_calendar.total_events &&
		               i == mc_clock_calendar.current_event);
		uint16_t bg = sel ? WHITE : BLACK;
		uint16_t fg = sel ? BLACK : WHITE;

		view_render.fillRect(0, y, LCD_WIDTH, 17, bg);
		view_render.setTextColor(fg);

		/* Line 1: category + alarm indicator */
		view_render.setCursor(4, y + 2);
		view_render.print(mc_clock_calendar_category_name[ev->category]);

		if (ev->alarm_enabled)
		{
			view_render.drawRoundRect(114, y + 2, 10, 8, 2, fg);
		}

		/* Line 2: time range */
		xsprintf(buf, "%02u:%02u - %02u:%02u",
		         ev->start_hour, ev->start_min,
		         ev->end_hour, ev->end_min);
		view_render.setCursor(4, y + 10);
		view_render.print(buf);

		view_render.setTextColor(WHITE);

		shown++;
		ev_idx++;
	}

	if (shown == 0)
	{
		view_render.setCursor(16, 26);
		view_render.print("No events today");
	}

	/* Bottom hint */
	view_render.drawLine(0, 50, LCD_WIDTH - 1, 50, WHITE);
	view_render.setCursor(2, 54);
	view_render.print("Mode: Edit");
}

/*****************************************************************************/
/* View — Edit event                                                          */
/*****************************************************************************/

static const char* const FIELD_LABEL[CALENDAR_FIELD_MAX] = {
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

static void view_scr_calendar_edit()
{
	if (mc_clock_calendar.editing_event >= mc_clock_calendar.total_events)
	{
		view_render.setCursor(25, 30);
		view_render.print("NO EVENT");
		return;
	}

	const mc_clock_calendar_event_t* ev = &mc_clock_calendar.events[mc_clock_calendar.editing_event];
	char buf[20];

	view_render.drawRoundRect(0, 0, LCD_WIDTH, LCD_HEIGHT, 4, WHITE);

	/* Title */
	view_render.setTextSize(1);
	view_render.setCursor(4, 4);
	view_render.print(mc_clock_calendar.is_new_event ? "Add Event" : "Edit Event");

	/* Field label */
	view_render.setCursor(4, 16);
	if (mc_clock_calendar.editing_field < CALENDAR_FIELD_MAX)
	{
		view_render.print(FIELD_LABEL[mc_clock_calendar.editing_field]);
	}

	/* Current value */
	view_render.setTextSize(2);
	view_render.setCursor(4, 28);

	switch (mc_clock_calendar.editing_field)
	{
	case CALENDAR_FIELD_YEAR:
		xsprintf(buf, "%u", ev->year);
		break;
	case CALENDAR_FIELD_MONTH:
		xsprintf(buf, "%s", MONTH_NAME[ev->month]);
		break;
	case CALENDAR_FIELD_DAY:
		xsprintf(buf, "%u", ev->day);
		break;
	case CALENDAR_FIELD_CATEGORY:
		xsprintf(buf, "%s", mc_clock_calendar_category_name[ev->category]);
		break;
	case CALENDAR_FIELD_START_HOUR:
	case CALENDAR_FIELD_END_HOUR:
		xsprintf(buf, "%02u", (mc_clock_calendar.editing_field == CALENDAR_FIELD_START_HOUR) ? ev->start_hour : ev->end_hour);
		break;
	case CALENDAR_FIELD_START_MIN:
	case CALENDAR_FIELD_END_MIN:
		xsprintf(buf, "%02u", (mc_clock_calendar.editing_field == CALENDAR_FIELD_START_MIN) ? ev->start_min : ev->end_min);
		break;
	case CALENDAR_FIELD_ALARM:
		xsprintf(buf, "%s", ev->alarm_enabled ? "ON" : "OFF");
		break;
	default:
		buf[0] = '\0';
		break;
	}

	view_render.print(buf);
	view_render.setTextSize(1);

	/* Progress bar */
	uint8_t bar_w = (uint8_t)(((uint16_t)(mc_clock_calendar.editing_field + 1) * 110) / CALENDAR_FIELD_MAX);
	view_render.fillRect(8, 48, bar_w, 4, WHITE);

	/* Hint */
	view_render.setCursor(4, 54);
	view_render.print("UP/DN:val  MD:next");
}

/*****************************************************************************/
/* View — Ringing overlay                                                     */
/*****************************************************************************/

static void view_scr_calendar_ringing()
{
	char buf[20];

	view_render.drawRoundRect(8, 8, 112, 48, 5, WHITE);
	view_render.fillRect(10, 10, 108, 44, BLACK);
	view_render.drawRoundRect(10, 10, 108, 44, 4, WHITE);

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	view_render.setCursor(32, 14);
	view_render.print("* REMINDER *");

	if (mc_clock_calendar.ringing_event < mc_clock_calendar.total_events)
	{
		const mc_clock_calendar_event_t* ev = &mc_clock_calendar.events[mc_clock_calendar.ringing_event];

		view_render.setCursor(14, 28);
		view_render.print(mc_clock_calendar_category_name[ev->category]);

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

static void view_scr_clock_calendar();

view_dynamic_t dyn_view_scr_clock_calendar = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_calendar};

view_screen_t scr_clock_calendar = {
    &dyn_view_scr_clock_calendar,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

static void view_scr_clock_calendar()
{
	/*
	 * Always redraw.
	 * Calendar is small (128x64), avoiding skipped frames
	 * prevents blank pages after mode changes.
	 */

	view_render.clear();
	view_render.setTextColor(WHITE);

	if (mc_clock_calendar.ringing)
	{
		view_scr_calendar_ringing();
		return;
	}

	switch (mc_clock_calendar.mode)
	{
	case CALENDAR_MODE_MONTH:
		view_scr_calendar_month();
		break;

	case CALENDAR_MODE_LIST:
		view_scr_calendar_list();
		break;

	case CALENDAR_MODE_EDIT:
		view_scr_calendar_edit();
		break;

	default:
		view_scr_calendar_month();
		break;
	}
}

void scr_clock_calendar_handle(ak_msg_t* msg)
{
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
		APP_DBG_SIG("MC_CLOCK_TIME_TICK [calendar]\n");
		/* Event alarms are checked on the RTC 1 Hz tick; this tick only refreshes the view */
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED [calendar]\n");
		if (mc_clock_calendar.ringing)
		{
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_ALARM_DISMISS);
			break;
		}

		switch (mc_clock_calendar.mode)
		{
		case CALENDAR_MODE_MONTH:
		{
			task_post_pure_msg(
			    MC_CLOCK_CALENDAR_ID,
			    MC_CLOCK_CALENDAR_EDIT_EVENT);

			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		case CALENDAR_MODE_LIST:
		{
			task_post_pure_msg(
			    MC_CLOCK_CALENDAR_ID,
			    MC_CLOCK_CALENDAR_EDIT_EVENT);

			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		case CALENDAR_MODE_EDIT:
		{
			task_post_pure_msg(
			    MC_CLOCK_CALENDAR_ID,
			    MC_CLOCK_CALENDAR_NEXT_FIELD);

			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;
		default:
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTTON_LONG_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_LONG_MODE_PRESSED [calendar]\n");
		if (mc_clock_calendar.ringing)
		{
			break;
		}

		switch (mc_clock_calendar.mode)
		{
		case CALENDAR_MODE_MONTH:
		{
			/* Add new event */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
			                   MC_CLOCK_CALENDAR_ADD_EVENT);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		case CALENDAR_MODE_LIST:
		{
			/* Delete selected event */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
			                   MC_CLOCK_CALENDAR_DELETE_EVENT);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		case CALENDAR_MODE_EDIT:
		{
			if (mc_clock_calendar.is_new_event)
			{
				/* Cancel new event */
				task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
				                   MC_CLOCK_CALENDAR_DELETE_EVENT);
			}
			else
			{
				/* Finish editing: step NEXT_FIELD through the remaining fields;
				 * the one after the last field saves and leaves edit mode. */
				for (uint8_t field = mc_clock_calendar.editing_field; field < CALENDAR_FIELD_MAX; field++)
				{
					task_post_pure_msg(MC_CLOCK_CALENDAR_ID,
					                   MC_CLOCK_CALENDAR_NEXT_FIELD);
				}
			}

			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
		break;

		default:
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED [calendar]\n");
		if (mc_clock_calendar.ringing)
		{
			break;
		}

		if (mc_clock_calendar.mode == CALENDAR_MODE_MONTH)
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

	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED [calendar]\n");
		if (mc_clock_calendar.ringing)
		{
			break;
		}

		if (mc_clock_calendar.mode == CALENDAR_MODE_MONTH)
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

	case AC_DISPLAY_BUTTON_LONG_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_LONG_UP_PRESSED [calendar]\n");
		if (mc_clock_calendar.mode == CALENDAR_MODE_MONTH)
		{
			/* Long-UP: previous month */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_PREV_MONTH);
		}
		else if (mc_clock_calendar.mode == CALENDAR_MODE_LIST)
		{
			/* Long-UP in list: back to month view */
			task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_SETUP);
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_LONG_DOWN_PRESSED [calendar]\n");
		if (mc_clock_calendar.mode == CALENDAR_MODE_MONTH)
		{
			task_post_pure_msg(
			    MC_CLOCK_CALENDAR_ID,
			    MC_CLOCK_CALENDAR_NEXT_MONTH);
		}
		else
		{
			task_post_pure_msg(
			    MC_CLOCK_CALENDAR_ID,
			    MC_CLOCK_CALENDAR_BACK);
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_UP_PRESSED [calendar]\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
		SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
		BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
	}
	break;

	default:
		break;
	}
}
