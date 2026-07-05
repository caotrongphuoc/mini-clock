#include "scr_clock_setting.h"

static rtc_time_t s_scr_clock_setting_time;
static rtc_date_t s_scr_clock_setting_date;
static uint8_t s_scr_clock_setting_field = SCR_CLOCK_SETTING_YEAR;

static void view_scr_clock_setting();
static void scr_clock_setting_load_state();
static void scr_clock_setting_draw_field_name();
static void scr_clock_setting_draw_value();
static void scr_clock_setting_write_2_digit(char* buffer, uint8_t value);
static void scr_clock_setting_write_4_digit(char* buffer, uint16_t value);
static uint8_t scr_clock_setting_days_in_month(uint16_t year, uint8_t month);
static void scr_clock_setting_clamp_date();
static void scr_clock_setting_increase_field();
static void scr_clock_setting_decrease_field();
static void scr_clock_setting_next_field();
static void scr_clock_setting_save();

view_dynamic_t dyn_view_scr_clock_setting = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_setting};

view_screen_t scr_clock_setting = {
    &dyn_view_scr_clock_setting,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

static void view_scr_clock_setting()
{
	view_render.clear();
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	scr_clock_setting_draw_field_name();
	scr_clock_setting_draw_value();
}

static void scr_clock_setting_load_state()
{
	mc_clock_clock_state_t clock_state;

	mc_clock_clock_get_state(&clock_state);
	s_scr_clock_setting_time = clock_state.time;
	s_scr_clock_setting_date = clock_state.date;
	scr_clock_setting_clamp_date();
}

static void scr_clock_setting_draw_field_name()
{
	static const char* const field_name[] = {
	    "YEAR", "MONTH", "DATE", "WEEKDAY", "HOUR", "MIN", "SEC", "SAVE"};

	view_render.setCursor(0, 0);
	view_render.print("SET ");
	view_render.print(field_name[s_scr_clock_setting_field]);
}

static void scr_clock_setting_draw_value()
{
	char date_text[11];
	char time_text[9];
	char weekday_text[2];

	scr_clock_setting_write_4_digit(&date_text[0], s_scr_clock_setting_date.year);
	date_text[4] = '-';
	scr_clock_setting_write_2_digit(&date_text[5], s_scr_clock_setting_date.month);
	date_text[7] = '-';
	scr_clock_setting_write_2_digit(&date_text[8], s_scr_clock_setting_date.date);
	date_text[10] = '\0';

	scr_clock_setting_write_2_digit(&time_text[0], s_scr_clock_setting_time.hour);
	time_text[2] = ':';
	scr_clock_setting_write_2_digit(&time_text[3], s_scr_clock_setting_time.min);
	time_text[5] = ':';
	scr_clock_setting_write_2_digit(&time_text[6], s_scr_clock_setting_time.sec);
	time_text[8] = '\0';

	weekday_text[0] = s_scr_clock_setting_date.weekday + '0';
	weekday_text[1] = '\0';

	view_render.setTextSize(1);
	view_render.setCursor(16, 18);
	view_render.print(date_text);
	view_render.setCursor(16, 32);
	view_render.print(time_text);
	view_render.setCursor(16, 46);
	view_render.print("WEEKDAY ");
	view_render.print(weekday_text);

	if (s_scr_clock_setting_field == SCR_CLOCK_SETTING_SAVE)
	{
		view_render.setTextColor(BLACK);
		view_render.fillRect(84, 50, 36, 10, WHITE);
		view_render.setCursor(90, 51);
		view_render.print("SAVE");
		view_render.setTextColor(WHITE);
	}
}

static void scr_clock_setting_write_2_digit(char* buffer, uint8_t value)
{
	buffer[0] = (value / 10) + '0';
	buffer[1] = (value % 10) + '0';
}

static void scr_clock_setting_write_4_digit(char* buffer, uint16_t value)
{
	buffer[0] = ((value / 1000) % 10) + '0';
	buffer[1] = ((value / 100) % 10) + '0';
	buffer[2] = ((value / 10) % 10) + '0';
	buffer[3] = (value % 10) + '0';
}

static uint8_t scr_clock_setting_days_in_month(uint16_t year, uint8_t month)
{
	static const uint8_t days[] = {
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

static void scr_clock_setting_clamp_date()
{
	uint8_t max_date = scr_clock_setting_days_in_month(s_scr_clock_setting_date.year,
	                                                   s_scr_clock_setting_date.month);

	if (s_scr_clock_setting_date.date > max_date)
	{
		s_scr_clock_setting_date.date = max_date;
	}
}

static void scr_clock_setting_increase_field()
{
	switch (s_scr_clock_setting_field)
	{
	case SCR_CLOCK_SETTING_YEAR:
		s_scr_clock_setting_date.year++;
		if (s_scr_clock_setting_date.year > SCR_CLOCK_SETTING_YEAR_MAX)
		{
			s_scr_clock_setting_date.year = SCR_CLOCK_SETTING_YEAR_MIN;
		}
		scr_clock_setting_clamp_date();
		break;

	case SCR_CLOCK_SETTING_MONTH:
		s_scr_clock_setting_date.month++;
		if (s_scr_clock_setting_date.month > 12)
		{
			s_scr_clock_setting_date.month = 1;
		}
		scr_clock_setting_clamp_date();
		break;

	case SCR_CLOCK_SETTING_DATE:
		s_scr_clock_setting_date.date++;
		if (s_scr_clock_setting_date.date > scr_clock_setting_days_in_month(s_scr_clock_setting_date.year,
		                                                                    s_scr_clock_setting_date.month))
		{
			s_scr_clock_setting_date.date = 1;
		}
		break;

	case SCR_CLOCK_SETTING_WEEKDAY:
		s_scr_clock_setting_date.weekday++;
		if (s_scr_clock_setting_date.weekday > RTC_WEEKDAY_SUN)
		{
			s_scr_clock_setting_date.weekday = RTC_WEEKDAY_MON;
		}
		break;

	case SCR_CLOCK_SETTING_HOUR:
		s_scr_clock_setting_time.hour = (s_scr_clock_setting_time.hour + 1) % 24;
		break;

	case SCR_CLOCK_SETTING_MIN:
		s_scr_clock_setting_time.min = (s_scr_clock_setting_time.min + 1) % 60;
		break;

	case SCR_CLOCK_SETTING_SEC:
		s_scr_clock_setting_time.sec = (s_scr_clock_setting_time.sec + 1) % 60;
		break;

	case SCR_CLOCK_SETTING_SAVE:
	default:
		break;
	}
}

static void scr_clock_setting_decrease_field()
{
	switch (s_scr_clock_setting_field)
	{
	case SCR_CLOCK_SETTING_YEAR:
		if (s_scr_clock_setting_date.year <= SCR_CLOCK_SETTING_YEAR_MIN)
		{
			s_scr_clock_setting_date.year = SCR_CLOCK_SETTING_YEAR_MAX;
		}
		else
		{
			s_scr_clock_setting_date.year--;
		}
		scr_clock_setting_clamp_date();
		break;

	case SCR_CLOCK_SETTING_MONTH:
		if (s_scr_clock_setting_date.month <= 1)
		{
			s_scr_clock_setting_date.month = 12;
		}
		else
		{
			s_scr_clock_setting_date.month--;
		}
		scr_clock_setting_clamp_date();
		break;

	case SCR_CLOCK_SETTING_DATE:
		if (s_scr_clock_setting_date.date <= 1)
		{
			s_scr_clock_setting_date.date = scr_clock_setting_days_in_month(s_scr_clock_setting_date.year,
			                                                                s_scr_clock_setting_date.month);
		}
		else
		{
			s_scr_clock_setting_date.date--;
		}
		break;

	case SCR_CLOCK_SETTING_WEEKDAY:
		if (s_scr_clock_setting_date.weekday <= RTC_WEEKDAY_MON)
		{
			s_scr_clock_setting_date.weekday = RTC_WEEKDAY_SUN;
		}
		else
		{
			s_scr_clock_setting_date.weekday--;
		}
		break;

	case SCR_CLOCK_SETTING_HOUR:
		s_scr_clock_setting_time.hour = (s_scr_clock_setting_time.hour == 0) ? 23 : s_scr_clock_setting_time.hour - 1;
		break;

	case SCR_CLOCK_SETTING_MIN:
		s_scr_clock_setting_time.min = (s_scr_clock_setting_time.min == 0) ? 59 : s_scr_clock_setting_time.min - 1;
		break;

	case SCR_CLOCK_SETTING_SEC:
		s_scr_clock_setting_time.sec = (s_scr_clock_setting_time.sec == 0) ? 59 : s_scr_clock_setting_time.sec - 1;
		break;

	case SCR_CLOCK_SETTING_SAVE:
	default:
		break;
	}
}

static void scr_clock_setting_next_field()
{
	if (s_scr_clock_setting_field == SCR_CLOCK_SETTING_SAVE)
	{
		scr_clock_setting_save();
		SCREEN_BACK();
		return;
	}

	s_scr_clock_setting_field++;
}

static void scr_clock_setting_save()
{
	rtc_set_date(&s_scr_clock_setting_date);
	rtc_set_time(&s_scr_clock_setting_time);
	task_post_pure_msg(MC_CLOCK_CLOCK_ID, MC_CLOCK_CLOCK_TICK);
}

void scr_clock_setting_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		s_scr_clock_setting_field = SCR_CLOCK_SETTING_YEAR;
		scr_clock_setting_load_state();
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		scr_clock_setting_next_field();
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		scr_clock_setting_increase_field();
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		scr_clock_setting_decrease_field();
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	default:
		break;
	}
}
