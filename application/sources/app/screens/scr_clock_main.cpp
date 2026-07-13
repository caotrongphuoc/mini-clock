#include "scr_clock_main.h"
#include "scr_clock_setting_display.h"

const char* scr_clock_main_weekday_short_text(uint8_t weekday)
{
	const char* weekday_text[] = {
	    "--", "MO", "TU", "WE", "TH", "FR", "SA", "SU"};

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

void scr_clock_main_format_time(char* buffer, rtc_time_t* time, uint8_t use_12h)
{
	uint8_t hour = time->hour;

	if (use_12h)
	{
		hour = hour % 12;
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

void scr_clock_main_draw_weekdays(uint8_t weekday, uint8_t inverted)
{
	view_render.setTextSize(1);

	for (uint8_t i = RTC_WEEKDAY_MON; i <= RTC_WEEKDAY_SUN; i++)
	{
		int16_t box_x = SCR_CLOCK_MAIN_WEEKDAY_AREA_X + ((i - RTC_WEEKDAY_MON) * SCR_CLOCK_MAIN_WEEKDAY_SEG_W);
		int16_t text_x = box_x + SCR_CLOCK_MAIN_WEEKDAY_TEXT_OFFSET_X;
		uint8_t selected = ((i == weekday) && inverted);

		if (selected)
		{
			view_render.fillRect(box_x,
			                     SCR_CLOCK_MAIN_WEEKDAY_BOX_Y,
			                     SCR_CLOCK_MAIN_WEEKDAY_BOX_W,
			                     SCR_CLOCK_MAIN_WEEKDAY_BOX_H,
			                     WHITE);
			view_render.setTextColor(BLACK);
		}
		else
		{
			view_render.setTextColor(WHITE);
		}

		view_render.setCursor(text_x, SCR_CLOCK_MAIN_WEEKDAY_ROW_Y);
		view_render.print(scr_clock_main_weekday_short_text(i));
	}

	view_render.setTextColor(WHITE);
}

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
	mc_clock_time_state_t state;
	char time_text[9];
	char date_text[11];

	mc_clock_time_get_state(&state);
	scr_clock_main_format_time(time_text, &state.time, scr_clock_setting_is_12h_format());
	scr_clock_main_format_date(date_text, &state.date);

	view_render.clear();
	view_render.setTextColor(WHITE);

	view_render.drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
	scr_clock_main_draw_weekdays(state.date.weekday, 1);

	view_render.drawRoundRect(SCR_CLOCK_MAIN_TIME_FRAME_X,
	                          SCR_CLOCK_MAIN_TIME_FRAME_Y,
	                          SCR_CLOCK_MAIN_TIME_FRAME_W,
	                          SCR_CLOCK_MAIN_TIME_FRAME_H,
	                          SCR_CLOCK_MAIN_TIME_FRAME_R,
	                          WHITE);
	view_render.setTextSize(SCR_CLOCK_MAIN_TIME_TEXT_SIZE);
	view_render.setCursor(SCR_CLOCK_MAIN_TIME_X, SCR_CLOCK_MAIN_TIME_Y);
	view_render.print(time_text);

	view_render.setTextSize(SCR_CLOCK_MAIN_INFO_TEXT_SIZE);
	view_render.setCursor(SCR_CLOCK_MAIN_DATE_TEXT_X, SCR_CLOCK_MAIN_DATE_TEXT_Y);
	view_render.print(date_text);
}

void scr_clock_main_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		mc_clock_time_update_local();
		timer_set(AC_TASK_DISPLAY_ID,
		          MC_CLOCK_TIME_TICK,
		          MC_CLOCK_TIME_TICK_INTERVAL,
		          TIMER_PERIODIC);
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
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
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
