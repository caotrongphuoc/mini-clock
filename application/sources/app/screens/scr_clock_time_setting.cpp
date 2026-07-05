#include "scr_clock_time_setting.h"

/*****************************************************************************/
/* Variable Declaration - Clock time setting */
/*****************************************************************************/

static rtc_time_t setting_time;
static rtc_date_t setting_date;
static uint8_t setting_location_choose = SCR_CLOCK_TIME_SETTING_YEAR;

/*****************************************************************************/
/* Value helper - Clock time setting */
/*****************************************************************************/

uint16_t scr_clock_time_setting_wrap_value(uint16_t value, uint16_t min, uint16_t max, int8_t step)
{
	if (step > 0)
	{
		return (value >= max) ? min : value + 1;
	}

	return (value <= min) ? max : value - 1;
}

uint8_t scr_clock_time_setting_days_in_month(uint16_t year, uint8_t month)
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

void scr_clock_time_setting_fix_date()
{
	uint8_t max_date = scr_clock_time_setting_days_in_month(setting_date.year,
	                                                        setting_date.month);

	if (setting_date.date > max_date)
	{
		setting_date.date = max_date;
	}
}

uint8_t scr_clock_time_setting_weekday(uint16_t year, uint8_t month, uint8_t date)
{
	if (month < 3)
	{
		month += 12;
		year--;
	}

	uint16_t weekday = (date + ((13 * (month + 1)) / 5) + year + (year / 4) - (year / 100) + (year / 400)) % 7;

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

void scr_clock_time_setting_change_value(int8_t step)
{
	switch (setting_location_choose)
	{
	case SCR_CLOCK_TIME_SETTING_YEAR:
		setting_date.year = scr_clock_time_setting_wrap_value(setting_date.year,
		                                                       SCR_CLOCK_TIME_SETTING_YEAR_MIN,
		                                                       SCR_CLOCK_TIME_SETTING_YEAR_MAX,
		                                                       step);
		scr_clock_time_setting_fix_date();
		break;

	case SCR_CLOCK_TIME_SETTING_MONTH:
		setting_date.month = scr_clock_time_setting_wrap_value(setting_date.month, 1, 12, step);
		scr_clock_time_setting_fix_date();
		break;

	case SCR_CLOCK_TIME_SETTING_DATE:
		setting_date.date = scr_clock_time_setting_wrap_value(setting_date.date,
		                                                      1,
		                                                      scr_clock_time_setting_days_in_month(setting_date.year,
		                                                                                           setting_date.month),
		                                                      step);
		break;

	case SCR_CLOCK_TIME_SETTING_WEEKDAY:
		setting_date.weekday = scr_clock_time_setting_wrap_value(setting_date.weekday,
		                                                         RTC_WEEKDAY_MON,
		                                                         RTC_WEEKDAY_SUN,
		                                                         step);
		break;

	case SCR_CLOCK_TIME_SETTING_HOUR:
		setting_time.hour = scr_clock_time_setting_wrap_value(setting_time.hour, 0, 23, step);
		break;

	case SCR_CLOCK_TIME_SETTING_MIN:
		setting_time.min = scr_clock_time_setting_wrap_value(setting_time.min, 0, 59, step);
		break;

	case SCR_CLOCK_TIME_SETTING_SEC:
		setting_time.sec = scr_clock_time_setting_wrap_value(setting_time.sec, 0, 59, step);
		break;

	case SCR_CLOCK_TIME_SETTING_SAVE:
	default:
		break;
	}
}

/*****************************************************************************/
/* View - Clock time setting */
/*****************************************************************************/

static void view_scr_clock_time_setting();

view_dynamic_t dyn_view_scr_clock_time_setting = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_time_setting};

view_screen_t scr_clock_time_setting = {
    &dyn_view_scr_clock_time_setting,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

void scr_clock_time_setting_write_2_digit(char* buffer, uint8_t value)
{
	buffer[0] = (value / 10) + '0';
	buffer[1] = (value % 10) + '0';
}

void scr_clock_time_setting_write_4_digit(char* buffer, uint16_t value)
{
	buffer[0] = ((value / 1000) % 10) + '0';
	buffer[1] = ((value / 100) % 10) + '0';
	buffer[2] = ((value / 10) % 10) + '0';
	buffer[3] = (value % 10) + '0';
}

void scr_clock_time_setting_print_text(int16_t x, int16_t y, const char* text, uint8_t text_size, uint8_t selected)
{
	uint8_t fg = selected ? BLACK : WHITE;
	uint8_t bg = selected ? WHITE : BLACK;
	uint8_t char_w = (text_size == SCR_CLOCK_MAIN_TIME_TEXT_SIZE) ?
	    SCR_CLOCK_TIME_SETTING_TIME_CHAR_W :
	    SCR_CLOCK_TIME_SETTING_INFO_CHAR_W;
	uint8_t char_h = (text_size == SCR_CLOCK_MAIN_TIME_TEXT_SIZE) ?
	    SCR_CLOCK_TIME_SETTING_TIME_CHAR_H :
	    SCR_CLOCK_TIME_SETTING_INFO_CHAR_H;
	uint8_t len = 0;

	while (text[len] != '\0')
	{
		len++;
	}

	if (selected)
	{
		view_render.fillRect(x - 1, y - 1, len * char_w + 2, char_h + 2, WHITE);
	}

	view_render.setTextSize(text_size);
	view_render.setTextColor(fg, bg);
	view_render.setCursor(x, y);
	view_render.print(text);
	view_render.setTextColor(WHITE);
}

void scr_clock_time_setting_print_weekday(uint8_t weekday)
{
	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_WEEKDAY_X,
	                                  SCR_CLOCK_MAIN_WEEKDAY_Y,
	                                  scr_clock_main_weekday_text(weekday),
	                                  SCR_CLOCK_MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == SCR_CLOCK_TIME_SETTING_WEEKDAY);
}

void scr_clock_time_setting_print_time(char* time_text)
{
	char hour_text[3] = {time_text[0], time_text[1], '\0'};
	char min_text[3] = {time_text[3], time_text[4], '\0'};
	char sec_text[3] = {time_text[6], time_text[7], '\0'};

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_TIME_X,
	                                  SCR_CLOCK_MAIN_TIME_Y,
	                                  hour_text,
	                                  SCR_CLOCK_MAIN_TIME_TEXT_SIZE,
	                                  setting_location_choose == SCR_CLOCK_TIME_SETTING_HOUR);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_TIME_X + 2 * SCR_CLOCK_TIME_SETTING_TIME_CHAR_W,
	                                  SCR_CLOCK_MAIN_TIME_Y,
	                                  ":",
	                                  SCR_CLOCK_MAIN_TIME_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_TIME_X + 3 * SCR_CLOCK_TIME_SETTING_TIME_CHAR_W,
	                                  SCR_CLOCK_MAIN_TIME_Y,
	                                  min_text,
	                                  SCR_CLOCK_MAIN_TIME_TEXT_SIZE,
	                                  setting_location_choose == SCR_CLOCK_TIME_SETTING_MIN);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_TIME_X + 5 * SCR_CLOCK_TIME_SETTING_TIME_CHAR_W,
	                                  SCR_CLOCK_MAIN_TIME_Y,
	                                  ":",
	                                  SCR_CLOCK_MAIN_TIME_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_TIME_X + 6 * SCR_CLOCK_TIME_SETTING_TIME_CHAR_W,
	                                  SCR_CLOCK_MAIN_TIME_Y,
	                                  sec_text,
	                                  SCR_CLOCK_MAIN_TIME_TEXT_SIZE,
	                                  setting_location_choose == SCR_CLOCK_TIME_SETTING_SEC);
}

void scr_clock_time_setting_print_date(char* date_text)
{
	char year_text[5] = {date_text[0], date_text[1], date_text[2], date_text[3], '\0'};
	char month_text[3] = {date_text[5], date_text[6], '\0'};
	char date_value_text[3] = {date_text[8], date_text[9], '\0'};

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_DATE_X,
	                                  SCR_CLOCK_MAIN_DATE_Y,
	                                  year_text,
	                                  SCR_CLOCK_MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == SCR_CLOCK_TIME_SETTING_YEAR);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_DATE_X + 4 * SCR_CLOCK_TIME_SETTING_INFO_CHAR_W,
	                                  SCR_CLOCK_MAIN_DATE_Y,
	                                  "-",
	                                  SCR_CLOCK_MAIN_INFO_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_DATE_X + 5 * SCR_CLOCK_TIME_SETTING_INFO_CHAR_W,
	                                  SCR_CLOCK_MAIN_DATE_Y,
	                                  month_text,
	                                  SCR_CLOCK_MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == SCR_CLOCK_TIME_SETTING_MONTH);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_DATE_X + 7 * SCR_CLOCK_TIME_SETTING_INFO_CHAR_W,
	                                  SCR_CLOCK_MAIN_DATE_Y,
	                                  "-",
	                                  SCR_CLOCK_MAIN_INFO_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(SCR_CLOCK_MAIN_DATE_X + 8 * SCR_CLOCK_TIME_SETTING_INFO_CHAR_W,
	                                  SCR_CLOCK_MAIN_DATE_Y,
	                                  date_value_text,
	                                  SCR_CLOCK_MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == SCR_CLOCK_TIME_SETTING_DATE);
}

void view_scr_clock_time_setting()
{
	char date_text[11];
	char time_text[9];

	scr_clock_time_setting_write_4_digit(&date_text[0], setting_date.year);
	date_text[4] = '-';
	scr_clock_time_setting_write_2_digit(&date_text[5], setting_date.month);
	date_text[7] = '-';
	scr_clock_time_setting_write_2_digit(&date_text[8], setting_date.date);
	date_text[10] = '\0';

	scr_clock_time_setting_write_2_digit(&time_text[0], setting_time.hour);
	time_text[2] = ':';
	scr_clock_time_setting_write_2_digit(&time_text[3], setting_time.min);
	time_text[5] = ':';
	scr_clock_time_setting_write_2_digit(&time_text[6], setting_time.sec);
	time_text[8] = '\0';

	view_render.clear();
	view_render.setTextColor(WHITE);

	scr_clock_time_setting_print_weekday(setting_date.weekday);
	scr_clock_time_setting_print_time(time_text);
	scr_clock_time_setting_print_date(date_text);

	if (setting_location_choose == SCR_CLOCK_TIME_SETTING_SAVE)
	{
		view_render.fillRect(88, 0, 36, 10, WHITE);
		view_render.setTextColor(BLACK);
		view_render.setTextSize(1);
		view_render.setCursor(94, 1);
		view_render.print("SAVE");
		view_render.setTextColor(WHITE);
	}
}

/*****************************************************************************/
/* Handle - Clock time setting */
/*****************************************************************************/

void scr_clock_time_setting_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		mc_clock_clock_state_t clock_state;

		mc_clock_clock_get_state(&clock_state);
		setting_time = clock_state.time;
		setting_date = clock_state.date;
		setting_location_choose = SCR_CLOCK_TIME_SETTING_YEAR;
		scr_clock_time_setting_fix_date();
		if (setting_date.weekday < RTC_WEEKDAY_MON || setting_date.weekday > RTC_WEEKDAY_SUN)
		{
			setting_date.weekday = scr_clock_time_setting_weekday(setting_date.year,
			                                                      setting_date.month,
			                                                      setting_date.date);
		}
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		if (setting_location_choose == SCR_CLOCK_TIME_SETTING_SAVE)
		{
			rtc_set_date(&setting_date);
			rtc_set_time(&setting_time);
			task_post_pure_msg(MC_CLOCK_CLOCK_ID, MC_CLOCK_CLOCK_TICK);
			SCREEN_BACK();
		}
		else
		{
			setting_location_choose++;
		}
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		scr_clock_time_setting_change_value(1);
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		scr_clock_time_setting_change_value(-1);
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	default:
		break;
	}
}
