#include "mc_sample_time.h"

static void view_scr_sample();

view_dynamic_t dyn_view_sample = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_sample};

view_screen_t mc_sample_time = {
    &dyn_view_sample,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

int item_selected = 0;  // 0: Time, 1: Date, 2: Country, 3: Exit
int country_index = 0;  // index for country selection
bool edit_mode = false; // false if not editing and true for editing

int edit_hour;
int edit_min;
int edit_sec;

int edit_year;
int edit_month;
int edit_day;

// const char* scr_clock_main_weekday_text(uint8_t weekday)
// {
// 	const char* weekday_text[] = {
// 	    "---", "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};

// 	if (weekday > RTC_WEEKDAY_SUN)
// 	{
// 		return weekday_text[0];
// 	}

// 	return weekday_text[weekday];
// }

// void scr_clock_main_write_2_digit(char* buffer, uint8_t value)
// {
// 	buffer[0] = (value / 10) + '0';
// 	buffer[1] = (value % 10) + '0';
// }

// void scr_clock_main_write_4_digit(char* buffer, uint16_t value)
// {
// 	buffer[0] = ((value / 1000) % 10) + '0';
// 	buffer[1] = ((value / 100) % 10) + '0';
// 	buffer[2] = ((value / 10) % 10) + '0';
// 	buffer[3] = (value % 10) + '0';
// }

// void scr_clock_main_format_time(char* buffer, rtc_time_t* time)
// {
// 	scr_clock_main_write_2_digit(&buffer[0], time->hour);
// 	buffer[2] = ':';
// 	scr_clock_main_write_2_digit(&buffer[3], time->min);
// 	buffer[5] = ':';
// 	scr_clock_main_write_2_digit(&buffer[6], time->sec);
// 	buffer[8] = '\0';
// }

// void scr_clock_main_format_date(char* buffer, rtc_date_t* date)
// {
// 	scr_clock_main_write_4_digit(&buffer[0], date->year);
// 	buffer[4] = '-';
// 	scr_clock_main_write_2_digit(&buffer[5], date->month);
// 	buffer[7] = '-';
// 	scr_clock_main_write_2_digit(&buffer[8], date->date);
// 	buffer[10] = '\0';
// }

void draw_menu_item(const char* text, int x, int y, bool selected)
{
	int width = 32;
	int height = 12;

	if (selected)
	{
		view_render.fillRoundRect(x, y, width, height, 3, WHITE);
		view_render.setTextColor(BLACK);
	}
	else
	{
		view_render.drawRoundRect(x, y, width, height, 3, WHITE);
		view_render.setTextColor(WHITE);
	}

	uint16_t text_width = strlen(text) * 6;
	uint16_t text_height = 8;

	view_render.setCursor(
	    x + (width - text_width) / 2,
	    y + (height - text_height) / 2);

	view_render.print(text);

	view_render.setTextColor(WHITE);
}

void time_option()
{
	char time_text[9];

	if (edit_mode)
	{
		sprintf(time_text,
		        "%02d:%02d:%02d",
		        edit_hour,
		        edit_min,
		        edit_sec);
	}
	else
	{
		mc_clock_time_state_t clock_state;

		mc_clock_time_get_state(&clock_state);

		sprintf(time_text,
		        "%02d:%02d:%02d",
		        clock_state.time.hour,
		        clock_state.time.min,
		        clock_state.time.sec);
	}

	view_render.print(time_text);
}

void date_option()
{
	char date_text[11];

	if (edit_mode)
	{
		sprintf(date_text,
		        "%04d-%02d-%02d",
		        edit_year,
		        edit_month,
		        edit_day);
	}
	else
	{
		mc_clock_time_state_t clock_state;

		mc_clock_time_get_state(&clock_state);

		sprintf(date_text,
		        "%04d-%02d-%02d",
		        clock_state.date.year,
		        clock_state.date.month,
		        clock_state.date.date);
	}

	view_render.print(date_text);
}

void country_option()
{
	const char* countries[] =
	    {
	        "Vietnam",
	        "Japan"};

	view_render.print(countries[country_index]);
}

void view_scr_sample()
{
	view_render.clear();

	// Draw title
	view_render.setCursor(40, 5);
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	if (item_selected == 0)
		view_render.print("Time");
	else if (item_selected == 1)
		view_render.print("Date");
	else if (item_selected == 2)
		view_render.print("Country");
	else
		view_render.print("Exit");

	if (edit_mode)
	{
		view_render.setCursor(90, 5);
		view_render.setTextSize(1);
		view_render.print("EDIT");
	}

	// Draw selected value
	view_render.setCursor(2, 20);
	view_render.setTextSize(2);

	if (item_selected == 0)
		time_option();
	else if (item_selected == 1)
		date_option();
	else if (item_selected == 2)
		country_option();

	view_render.setTextSize(1);

	draw_menu_item("Time", 0, 48, item_selected == 0);
	draw_menu_item("Date", 32, 48, item_selected == 1);
	draw_menu_item("Cntry", 64, 48, item_selected == 2);
	draw_menu_item("Exit", 96, 48, item_selected == 3);
}

void mc_sample_time_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		view_render_display_on();
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_LOGO, AC_DISPLAY_STARTUP_INTERVAL, TIMER_ONE_SHOT);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		if (!edit_mode)
		{
			if (item_selected == 3)
			{
				SCREEN_TRAN(scr_clock_main_handle, &scr_clock_main);
			}
			else
			{
				mc_clock_time_state_t clock_state;

				mc_clock_time_get_state(&clock_state);

				edit_hour = clock_state.time.hour;
				edit_min = clock_state.time.min;
				edit_sec = clock_state.time.sec;

				edit_year = clock_state.date.year;
				edit_month = clock_state.date.month;
				edit_day = clock_state.date.date;

				edit_mode = true;
			}
		}
		else
		{
			// SAVE HERE
			edit_mode = false;
		}
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		if (!edit_mode)
		{
			if (item_selected > 0)
				item_selected--;
		}
		else
		{
			// Editing value
			switch (item_selected)
			{
			case 0:
				edit_hour++;

				if (edit_hour > 23)
					edit_hour = 0;
				break;

			case 1:
				// increase date here
				break;

			case 2:
				// next country
				country_index++;
				if (country_index > 1)
					country_index = 0;
				break;
			}
		}
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		if (!edit_mode)
		{
			if (item_selected < 3)
				item_selected++;
		}
		else
		{
			// Editing value
			switch (item_selected)
			{
			case 0:
				edit_hour--;

				if (edit_hour < 0)
					edit_hour = 23;
				break;

			case 1:
				// decrease date here
				break;

			case 2:
				// previous country
				country_index--;

				if (country_index < 0)
					country_index = 1;
				break;
			}
		}
	}
	break;

	default:
		break;
	}
}
