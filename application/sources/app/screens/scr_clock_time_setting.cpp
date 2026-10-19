#include "scr_clock_time_setting.h"

/*****************************************************************************/
/* Variable Declaration - Clock time setting */
/*****************************************************************************/

static rtc_time_t setting_time;
static rtc_date_t setting_date;
static uint8_t setting_location_choose = TIME_SETTING_YEAR;

/*****************************************************************************/
/* Value helper - Clock time setting */
/*****************************************************************************/

uint16_t scr_clock_time_setting_wrap_value(uint16_t value, uint16_t min, uint16_t max, int8_t direction)
{
	if (direction > 0)
	{
		return (value >= max) ? min : value + 1;
	}

	return (value <= min) ? max : value - 1;
}

void scr_clock_time_setting_fix_date()
{
	uint8_t max_date = mc_clock_date_days_in_month(setting_date.year, setting_date.month);

	if (setting_date.date > max_date)
	{
		setting_date.date = max_date;
	}
}

void scr_clock_time_setting_change_value(int8_t step)
{
	switch (setting_location_choose)
	{
	case TIME_SETTING_YEAR:
		setting_date.year = scr_clock_time_setting_wrap_value(setting_date.year,
		                                                      TIME_SETTING_YEAR_MIN,
		                                                      TIME_SETTING_YEAR_MAX,
		                                                      step);
		scr_clock_time_setting_fix_date();
		break;

	case TIME_SETTING_MONTH:
		setting_date.month = scr_clock_time_setting_wrap_value(setting_date.month, 1, 12, step);
		scr_clock_time_setting_fix_date();
		break;

	case TIME_SETTING_DATE:
		setting_date.date = scr_clock_time_setting_wrap_value(setting_date.date,
		                                                      1,
		                                                      mc_clock_date_days_in_month(setting_date.year,
		                                                                                  setting_date.month),
		                                                      step);
		break;

	case TIME_SETTING_WEEKDAY:
		setting_date.weekday = scr_clock_time_setting_wrap_value(setting_date.weekday,
		                                                         RTC_WEEKDAY_MON,
		                                                         RTC_WEEKDAY_SUN,
		                                                         step);
		break;

	case TIME_SETTING_HOUR:
		setting_time.hour = scr_clock_time_setting_wrap_value(setting_time.hour, 0, 23, step);
		break;

	case TIME_SETTING_MIN:
		setting_time.min = scr_clock_time_setting_wrap_value(setting_time.min, 0, 59, step);
		break;

	case TIME_SETTING_SEC:
		setting_time.sec = scr_clock_time_setting_wrap_value(setting_time.sec, 0, 59, step);
		break;

	case TIME_SETTING_SAVE:
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

void scr_clock_time_setting_print_text(int16_t x, int16_t y, const char* text, uint8_t text_size, uint8_t selected)
{
	uint8_t fg = selected ? BLACK : WHITE;
	uint8_t bg = selected ? WHITE : BLACK;
	uint8_t char_w = (text_size == MAIN_TIME_TEXT_SIZE) ? TIME_SETTING_TIME_CHAR_W : TIME_SETTING_INFO_CHAR_W;
	uint8_t char_h = (text_size == MAIN_TIME_TEXT_SIZE) ? TIME_SETTING_TIME_CHAR_H : TIME_SETTING_INFO_CHAR_H;
	uint8_t len = 0;

	while (text[len] != '\0')
	{
		len++;
	}

	if (selected)
	{
		view_render.fillRect(x - MAIN_SELECT_PAD_X,
		                     y - MAIN_SELECT_PAD_Y,
		                     len * char_w + (MAIN_SELECT_PAD_X * 2),
		                     char_h + (MAIN_SELECT_PAD_Y * 2),
		                     WHITE);
	}

	view_render.setTextSize(text_size);
	view_render.setTextColor(fg, bg);
	view_render.setCursor(x, y);
	view_render.print(text);
	view_render.setTextColor(WHITE);
}

void scr_clock_time_setting_print_time(char* time_text)
{
	char hour_text[3] = {time_text[0], time_text[1], '\0'};
	char min_text[3] = {time_text[3], time_text[4], '\0'};
	char sec_text[3] = {time_text[6], time_text[7], '\0'};

	scr_clock_time_setting_print_text(MAIN_TIME_X,
	                                  MAIN_TIME_Y,
	                                  hour_text,
	                                  MAIN_TIME_TEXT_SIZE,
	                                  setting_location_choose == TIME_SETTING_HOUR);

	scr_clock_time_setting_print_text(MAIN_TIME_X + 2 * TIME_SETTING_TIME_CHAR_W,
	                                  MAIN_TIME_Y,
	                                  ":",
	                                  MAIN_TIME_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(MAIN_TIME_X + 3 * TIME_SETTING_TIME_CHAR_W,
	                                  MAIN_TIME_Y,
	                                  min_text,
	                                  MAIN_TIME_TEXT_SIZE,
	                                  setting_location_choose == TIME_SETTING_MIN);

	scr_clock_time_setting_print_text(MAIN_TIME_X + 5 * TIME_SETTING_TIME_CHAR_W,
	                                  MAIN_TIME_Y,
	                                  ":",
	                                  MAIN_TIME_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(MAIN_TIME_X + 6 * TIME_SETTING_TIME_CHAR_W,
	                                  MAIN_TIME_Y,
	                                  sec_text,
	                                  MAIN_TIME_TEXT_SIZE,
	                                  setting_location_choose == TIME_SETTING_SEC);
}

void scr_clock_time_setting_print_date(char* date_text)
{
	char year_text[5] = {date_text[0], date_text[1], date_text[2], date_text[3], '\0'};
	char month_text[3] = {date_text[5], date_text[6], '\0'};
	char date_value_text[3] = {date_text[8], date_text[9], '\0'};

	scr_clock_time_setting_print_text(MAIN_DATE_TEXT_X,
	                                  MAIN_DATE_TEXT_Y,
	                                  year_text,
	                                  MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == TIME_SETTING_YEAR);

	scr_clock_time_setting_print_text(MAIN_DATE_TEXT_X + 4 * TIME_SETTING_INFO_CHAR_W,
	                                  MAIN_DATE_TEXT_Y,
	                                  "-",
	                                  MAIN_INFO_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(MAIN_DATE_TEXT_X + 5 * TIME_SETTING_INFO_CHAR_W,
	                                  MAIN_DATE_TEXT_Y,
	                                  month_text,
	                                  MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == TIME_SETTING_MONTH);

	scr_clock_time_setting_print_text(MAIN_DATE_TEXT_X + 7 * TIME_SETTING_INFO_CHAR_W,
	                                  MAIN_DATE_TEXT_Y,
	                                  "-",
	                                  MAIN_INFO_TEXT_SIZE,
	                                  0);

	scr_clock_time_setting_print_text(MAIN_DATE_TEXT_X + 8 * TIME_SETTING_INFO_CHAR_W,
	                                  MAIN_DATE_TEXT_Y,
	                                  date_value_text,
	                                  MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == TIME_SETTING_DATE);
}

void scr_clock_time_setting_print_save()
{
	scr_clock_time_setting_print_text(TIME_SETTING_SAVE_X,
	                                  MAIN_DATE_TEXT_Y,
	                                  "OK",
	                                  MAIN_INFO_TEXT_SIZE,
	                                  setting_location_choose == TIME_SETTING_SAVE);
}

void view_scr_clock_time_setting()
{
	char date_text[11];
	char time_text[9];

	scr_clock_main_format_date(date_text, &setting_date);
	scr_clock_main_format_time(time_text, &setting_time, 0);

	view_render.clear();
	view_render.setTextColor(WHITE);

	view_render.drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
	scr_clock_main_draw_weekdays(setting_date.weekday,
	                             setting_location_choose == TIME_SETTING_WEEKDAY);

	view_render.drawRoundRect(MAIN_TIME_FRAME_X,
	                          MAIN_TIME_FRAME_Y,
	                          MAIN_TIME_FRAME_W,
	                          MAIN_TIME_FRAME_H,
	                          MAIN_TIME_FRAME_R,
	                          WHITE);
	scr_clock_time_setting_print_time(time_text);
	scr_clock_time_setting_print_date(date_text);
	scr_clock_time_setting_print_save();
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
		setting_time = mc_clock_time.time;
		setting_date = mc_clock_time.date;
		setting_location_choose = TIME_SETTING_YEAR;
		scr_clock_time_setting_fix_date();
		if (setting_date.weekday < RTC_WEEKDAY_MON || setting_date.weekday > RTC_WEEKDAY_SUN)
		{
			setting_date.weekday = mc_clock_date_weekday(setting_date.year,
			                                             setting_date.month,
			                                             setting_date.date);
		}
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		if (setting_location_choose == TIME_SETTING_SAVE)
		{
			rtc_time_t raw_time;
			rtc_date_t raw_date;
			mc_clock_time_adjust_timezone(&setting_time, &setting_date, -mc_clock_world_clock_get_selected_offset_minutes(), &raw_time, &raw_date);

			mc_clock_rtc_set_time_req_t req;
			req.time = raw_time;
			req.date = raw_date;
			req.apply_time = 1;
			req.apply_date = 1;
			task_post_common_msg(MC_CLOCK_RTC_ID,
			                     MC_CLOCK_RTC_SET_TIME_REQ,
			                     (uint8_t*)&req,
			                     sizeof(req));
			/* The RTC task re-arms the alarms once the new time is written */
			task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_UPDATE);
			SCREEN_BACK();
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
		}
		else
		{
			setting_location_choose++;
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
	}
	break;

	case AC_DISPLAY_BUTTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED\n");
		scr_clock_time_setting_change_value(1);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED\n");
		scr_clock_time_setting_change_value(-1);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}
