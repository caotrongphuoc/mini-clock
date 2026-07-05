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

void scr_clock_main_format_time(char* buffer, rtc_time_t* time, uint8_t format_24h)
{
	uint8_t hour = time->hour;

	if (!format_24h)
	{
		hour %= 12;
		if (hour == 0)
		{
			hour = 12;
		}
	}

	scr_clock_main_write_2_digit(&buffer[0], hour);
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
	mc_clock_clock_state_t clock_state;
	char time_text[9];
	char date_text[11];

	mc_clock_clock_get_state(&clock_state);
	scr_clock_main_format_time(time_text, &clock_state.time, clock_state.format_24h);
	scr_clock_main_format_date(date_text, &clock_state.date);

	view_render.clear();
	view_render.setTextColor(WHITE);

	view_render.setTextSize(SCR_CLOCK_MAIN_INFO_TEXT_SIZE);
	view_render.setCursor(SCR_CLOCK_MAIN_WEEKDAY_X, SCR_CLOCK_MAIN_WEEKDAY_Y);
	view_render.print(scr_clock_main_weekday_text(clock_state.date.weekday));

	view_render.setTextSize(SCR_CLOCK_MAIN_TIME_TEXT_SIZE);
	view_render.setCursor(SCR_CLOCK_MAIN_TIME_X, SCR_CLOCK_MAIN_TIME_Y);
	view_render.print(time_text);

	view_render.setTextSize(SCR_CLOCK_MAIN_INFO_TEXT_SIZE);
	view_render.setCursor(SCR_CLOCK_MAIN_DATE_X, SCR_CLOCK_MAIN_DATE_Y);
	view_render.print(date_text);
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
		task_post_pure_msg(MC_CLOCK_CLOCK_ID, MC_CLOCK_CLOCK_ENTER);
	}
	break;

	case SCREEN_EXIT:
	{
		task_post_pure_msg(MC_CLOCK_CLOCK_ID, MC_CLOCK_CLOCK_LEAVE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
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
