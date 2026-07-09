#include "scr_clock_main.h"

/*****************************************************************************/
/* View helper - Clock main */
/*****************************************************************************/

const char* scr_clock_main_weekday_text(uint8_t weekday)
{
	const char* weekday_text[] = {
	    "---", "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};

	if (weekday > RTC_WEEKDAY_SUN)
	{
		return weekday_text[0];
	}

	return weekday_text[weekday];
}

const char* scr_clock_main_month_text(uint8_t month)
{
	const char* month_text[] = {
	    "---", "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
	    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

	if (month == 0 || month > 12)
	{
		return month_text[0];
	}

	return month_text[month];
}

void scr_clock_main_write_2_digit(char* buffer, uint8_t value)
{
	buffer[0] = (value / 10) + '0';
	buffer[1] = (value % 10) + '0';
}

void scr_clock_main_write_4_digit(char* buffer, uint16_t value)
{
	buffer[0] = ((value / 1000) % 10) + '0';
	buffer[1] = ((value / 100) % 10) + '0';
	buffer[2] = ((value / 10) % 10) + '0';
	buffer[3] = (value % 10) + '0';
}

void scr_clock_main_format_time(char* buffer, rtc_time_t* time)
{
	scr_clock_main_write_2_digit(&buffer[0], time->hour);
	buffer[2] = ':';
	scr_clock_main_write_2_digit(&buffer[3], time->min);
	buffer[5] = ':';
	scr_clock_main_write_2_digit(&buffer[6], time->sec);
	buffer[8] = '\0';
}

void scr_clock_main_format_date(char* buffer, rtc_date_t* date)
{
	scr_clock_main_write_4_digit(&buffer[0], date->year);
	buffer[4] = '-';
	scr_clock_main_write_2_digit(&buffer[5], date->month);
	buffer[7] = '-';
	scr_clock_main_write_2_digit(&buffer[8], date->date);
	buffer[10] = '\0';
}

uint8_t scr_clock_main_days_in_month(uint16_t year, uint8_t month)
{
	const uint8_t days[] = {
	    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (month == 0 || month > 12)
	{
		return 31;
	}

	if (month == 2 && ((year % 4) == 0))
	{
		return 29;
	}

	return days[month - 1];
}

uint8_t scr_clock_main_weekday(uint16_t year, uint8_t month, uint8_t date)
{
	if (month < 3)
	{
		month += 12;
		year--;
	}

	uint16_t year_in_century = year % 100;
	uint16_t century = year / 100;
	uint16_t weekday = (date + ((13 * (month + 1)) / 5) +
	                    year_in_century + (year_in_century / 4) +
	                    (century / 4) + (5 * century)) % 7;

	if (weekday == 0)
	{
		return RTC_WEEKDAY_SUN - 1;
	}

	if (weekday == 1)
	{
		return RTC_WEEKDAY_SUN;
	}

	return weekday - 1;
}

void scr_clock_main_format_hour_min(char* buffer, rtc_time_t* time)
{
	scr_clock_main_write_2_digit(&buffer[0], time->hour);
	buffer[2] = ':';
	scr_clock_main_write_2_digit(&buffer[3], time->min);
	buffer[5] = '\0';
}

void scr_clock_main_draw_second_tick(uint8_t index, uint8_t second)
{
	int16_t x;
	int16_t y;
	uint8_t offset = index % 15;

	if (index < 15)
	{
		x = SCR_CLOCK_MAIN_FRAME_X + 4 + (offset * 4);
		y = SCR_CLOCK_MAIN_FRAME_Y;
		view_render.drawLine(x, y, x, y + (index == second ? 4 : 2), WHITE);
	}
	else if (index < 30)
	{
		x = SCR_CLOCK_MAIN_FRAME_X + SCR_CLOCK_MAIN_FRAME_W - 1;
		y = SCR_CLOCK_MAIN_FRAME_Y + 3 + (offset * 3);
		view_render.drawLine(x, y, x - (index == second ? 4 : 2), y, WHITE);
	}
	else if (index < 45)
	{
		x = SCR_CLOCK_MAIN_FRAME_X + SCR_CLOCK_MAIN_FRAME_W - 5 - (offset * 4);
		y = SCR_CLOCK_MAIN_FRAME_Y + SCR_CLOCK_MAIN_FRAME_H - 1;
		view_render.drawLine(x, y, x, y - (index == second ? 4 : 2), WHITE);
	}
	else
	{
		x = SCR_CLOCK_MAIN_FRAME_X;
		y = SCR_CLOCK_MAIN_FRAME_Y + SCR_CLOCK_MAIN_FRAME_H - 4 - (offset * 3);
		view_render.drawLine(x, y, x + (index == second ? 4 : 2), y, WHITE);
	}
}

void scr_clock_main_draw_second_frame(uint8_t second)
{
	for (uint8_t i = 0; i < 60; i++)
	{
		scr_clock_main_draw_second_tick(i, second);
	}
}

void scr_clock_main_draw_calendar_digit(uint8_t digit, int16_t x, int16_t y, uint8_t color)
{
	const uint8_t digit_map[10][5] = {
	    {0x07, 0x05, 0x05, 0x05, 0x07},
	    {0x02, 0x06, 0x02, 0x02, 0x07},
	    {0x07, 0x01, 0x07, 0x04, 0x07},
	    {0x07, 0x01, 0x07, 0x01, 0x07},
	    {0x05, 0x05, 0x07, 0x01, 0x01},
	    {0x07, 0x04, 0x07, 0x01, 0x07},
	    {0x07, 0x04, 0x07, 0x05, 0x07},
	    {0x07, 0x01, 0x01, 0x01, 0x01},
	    {0x07, 0x05, 0x07, 0x05, 0x07},
	    {0x07, 0x05, 0x07, 0x01, 0x07},
	};

	for (uint8_t row = 0; row < 5; row++)
	{
		for (uint8_t col = 0; col < 3; col++)
		{
			if (digit_map[digit][row] & (1 << (2 - col)))
			{
				view_render.drawPixel(x + col, y + row, color);
			}
		}
	}
}

void scr_clock_main_print_day(uint8_t day, uint8_t col, uint8_t row, uint8_t selected)
{
	int16_t x = SCR_CLOCK_MAIN_CAL_X + (col * SCR_CLOCK_MAIN_CAL_COL_STEP);
	int16_t y = SCR_CLOCK_MAIN_CAL_DAY_Y + (row * SCR_CLOCK_MAIN_CAL_ROW_STEP);
	uint8_t color = selected ? BLACK : WHITE;

	if (selected)
	{
		view_render.fillRect(x - 1, y - 1, 8, 7, WHITE);
	}

	if (day >= 10)
	{
		scr_clock_main_draw_calendar_digit(day / 10, x, y, color);
		scr_clock_main_draw_calendar_digit(day % 10, x + 4, y, color);
	}
	else
	{
		scr_clock_main_draw_calendar_digit(day, x + 2, y, color);
	}
}

void scr_clock_main_draw_calendar(rtc_date_t* date)
{
	const char* weekdays = "MTWTFSS";
	uint8_t first_weekday = scr_clock_main_weekday(date->year, date->month, 1);
	uint8_t days = scr_clock_main_days_in_month(date->year, date->month);
	uint8_t start_col = first_weekday - RTC_WEEKDAY_MON;

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(SCR_CLOCK_MAIN_CAL_X + 14, SCR_CLOCK_MAIN_CAL_MONTH_Y);
	view_render.print(scr_clock_main_month_text(date->month));

	for (uint8_t col = 0; col < 7; col++)
	{
		view_render.setCursor(SCR_CLOCK_MAIN_CAL_X + (col * SCR_CLOCK_MAIN_CAL_COL_STEP),
		                      SCR_CLOCK_MAIN_CAL_WEEKDAY_Y);
		view_render.print(weekdays[col]);
	}

	for (uint8_t day = 1; day <= days; day++)
	{
		uint8_t index = start_col + day - 1;
		uint8_t col = index % 7;
		uint8_t row = index / 7;

		if (row > 5)
		{
			break;
		}

		scr_clock_main_print_day(day, col, row, day == date->date);
	}
}

/*****************************************************************************/
/* View - Clock main */
/*****************************************************************************/

static void view_scr_clock_main();

view_dynamic_t dyn_view_scr_clock_main = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_main};

view_screen_t scr_clock_main = {
    &dyn_view_scr_clock_main,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

void view_scr_clock_main()
{
	mc_clock_time_state_t clock_state;
	char time_text[6];

	mc_clock_time_get_state(&clock_state);
	scr_clock_main_format_hour_min(time_text, &clock_state.time);

	view_render.clear();
	view_render.setTextColor(WHITE);

	scr_clock_main_draw_second_frame(clock_state.time.sec);

	view_render.setTextSize(SCR_CLOCK_MAIN_TIME_TEXT_SIZE);
	view_render.setCursor(SCR_CLOCK_MAIN_TIME_X, SCR_CLOCK_MAIN_TIME_Y);
	view_render.print(time_text);

	scr_clock_main_draw_calendar(&clock_state.date);
}

/*****************************************************************************/
/* Handle - Clock main */
/*****************************************************************************/

void scr_clock_main_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_SETUP);
		timer_set(AC_TASK_DISPLAY_ID,
		          MC_CLOCK_TIME_TICK,
		          MC_CLOCK_TIME_TICK_INTERVAL,
		          TIMER_PERIODIC);
	}
	break;

	case SCREEN_EXIT:
	{
		APP_DBG_SIG("SCREEN_EXIT\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
	}
	break;

	case MC_CLOCK_TIME_TICK:
	{
		APP_DBG_SIG("MC_CLOCK_TIME_TICK\n");
		task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
		SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_%s_PRESSED\n",
		            msg->sig == AC_DISPLAY_BUTON_UP_PRESSED ? "UP" : "DOWN");
	}
	break;

	default:
		break;
	}
}
