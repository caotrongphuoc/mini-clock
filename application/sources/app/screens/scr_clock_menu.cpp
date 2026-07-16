#include "scr_clock_menu.h"

#include "mc_clock_time.h"
#include "scr_clock_main.h"
#include "scr_clock_setting_display.h"

// animation variables
static uint8_t current_location = SCR_CLOCK_MENU_CLOCK;
static uint8_t target_location;
static int8_t animation_offset = 0;
static int8_t animation_direction = 0; // -1 = left, +1 = right
static uint8_t animation_running = 0;

static const unsigned char* const menu_bitmap[SCR_CLOCK_MENU_ITEM_NUMBER] = {
    bitmap_clock_menu_clock,
    bitmap_clock_menu_alarm,
    bitmap_clock_menu_stopwatch,
    bitmap_clock_menu_timer,
    bitmap_world_clock,
    bitmap_calender_display,
    bitmap_clock_menu_setting,
    bitmap_clock_menu_exit,
};

static const char* const menu_name[SCR_CLOCK_MENU_ITEM_NUMBER] = {
    "Clock",
    "Alarm",
    "Stopwatch",
    "Timer",
    "World Clock",
    "Calender",
    "Setting",
    "Exit",
};

static void view_scr_clock_menu();
view_dynamic_t dyn_view_scr_clock_menu = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_menu};

view_screen_t scr_clock_menu = {
    &dyn_view_scr_clock_menu,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_clock_menu_draw_frame()
{
	view_render.drawRoundRect(SCR_CLOCK_MENU_FRAME_X,
	                          SCR_CLOCK_MENU_FRAME_Y,
	                          SCR_CLOCK_MENU_FRAME_W,
	                          SCR_CLOCK_MENU_FRAME_H,
	                          SCR_CLOCK_MENU_FRAME_R,
	                          WHITE);
}

void scr_clock_menu_draw_icon()
{
	view_render.drawBitmap(SCR_CLOCK_MENU_ICON_X,
	                       SCR_CLOCK_MENU_ICON_Y,
	                       menu_bitmap[current_location],
	                       SCR_CLOCK_MENU_ICON_W,
	                       SCR_CLOCK_MENU_ICON_H,
	                       WHITE);
}

// animation functions
void scr_clock_menu_draw_icon_xy(int16_t x,
                                 int16_t y,
                                 uint8_t index)
{
	view_render.drawBitmap(x,
	                       y,
	                       menu_bitmap[index],
	                       SCR_CLOCK_MENU_ICON_W,
	                       SCR_CLOCK_MENU_ICON_H,
	                       WHITE);
}

// Draw the name of the menu item at a specific x position
void scr_clock_menu_draw_name_xy(int16_t x,
                                 uint8_t index)
{
	const char* name = menu_name[index];

	uint8_t len = 0;

	while (name[len])
		len++;

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	view_render.setCursor(x - len * 3,
	                      42);

	view_render.print(name);
}

void scr_clock_menu_draw_arrows()
{
	view_render.fillTriangle(8, 32, 15, 25, 15, 39, WHITE);
	view_render.fillTriangle(119, 32, 112, 25, 112, 39, WHITE);
}

void scr_clock_menu_draw_name()
{
	const char* name = menu_name[current_location];
	uint8_t len = 0;

	while (name[len] != '\0')
	{
		len++;
	}

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor((LCD_WIDTH - len * 6) / 2, 42);
	view_render.print(name);
}

void scr_clock_menu_draw_indicator()
{
	const uint8_t spacing = 10;
	const uint8_t count = SCR_CLOCK_MENU_ITEM_NUMBER;

	int16_t start_x = (LCD_WIDTH - ((count - 1) * spacing)) / 2;
	int16_t y = 55;

	for (uint8_t i = 0; i < count; i++)
	{
		int16_t x = start_x + i * spacing;

		if (i == current_location)
		{
			view_render.fillRoundRect(x - 2, y - 2, 5, 5, 1, WHITE);
		}
		else
		{
			view_render.drawCircle(x, y, 2, WHITE);
		}
	}
}

// void scr_clock_menu_draw_buttons()
// {
// 	view_render.drawRoundRect(8, 53, 36, 9, 2, WHITE);
// 	view_render.drawRoundRect(46, 53, 36, 9, 2, WHITE);
// 	view_render.drawRoundRect(84, 53, 36, 9, 2, WHITE);

// 	view_render.setTextSize(1);
// 	view_render.setTextColor(WHITE);

// 	view_render.setCursor(14, 54);
// 	view_render.print("Left");

// 	view_render.setCursor(49, 54);
// 	view_render.print("Right");

// 	view_render.setCursor(87, 54);
// 	view_render.print("Enter");
// }

void scr_clock_menu_draw_time()
{
	mc_clock_time_state_t state;
	char time_text[12];
	uint8_t use_12h = scr_clock_setting_is_12h_format();

	mc_clock_time_get_state(&state);
	scr_clock_main_format_time(time_text, &state.time, use_12h);

	if (use_12h)
	{
		time_text[8] = ' ';
		time_text[9] = (state.time.hour >= 12) ? 'P' : 'A';
		time_text[10] = 'M';
		time_text[11] = '\0';
	}

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(use_12h ? 31 : 40, 3);
	view_render.print(time_text);
}

void view_scr_clock_menu()
{
	APP_DBG("VIEW START\n");
	view_render.clear();

	view_render.drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);

	scr_clock_menu_draw_time();
	scr_clock_menu_draw_frame();

	int16_t center = SCR_CLOCK_MENU_ICON_X;

	if (animation_running)
	{
		scr_clock_menu_draw_icon_xy(
		    center + animation_direction * animation_offset,
		    SCR_CLOCK_MENU_ICON_Y,
		    current_location);

		scr_clock_menu_draw_name_xy(
		    center + animation_direction * animation_offset + 16,
		    current_location);

		scr_clock_menu_draw_icon_xy(
		    center + animation_direction * animation_offset +
		        (-animation_direction * 64),
		    SCR_CLOCK_MENU_ICON_Y,
		    target_location);

		scr_clock_menu_draw_name_xy(
		    center + animation_direction * animation_offset +
		        (-animation_direction * 64) + 16,
		    target_location);
	}
	else
	{
		scr_clock_menu_draw_icon();
		scr_clock_menu_draw_name();
	}

	scr_clock_menu_draw_arrows();
	scr_clock_menu_draw_indicator();
	APP_DBG("VIEW END\n");
}

void scr_clock_menu_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		current_location = SCR_CLOCK_MENU_CLOCK;
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

		if (animation_running)
		{
			animation_offset += MENU_ANIMATION_STEP;

			if (animation_offset >= MENU_ANIMATION_END)
			{
				current_location = target_location;
				animation_running = 0;
				animation_offset = 0;
			}
		}
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		switch (current_location)
		{
		case SCR_CLOCK_MENU_CLOCK:
			SCREEN_TRAN(scr_clock_main_handle, &scr_clock_main);
			break;

		case SCR_CLOCK_MENU_ALARM:
			SCREEN_TRAN(scr_clock_alarm_handle, &scr_clock_alarm);
			break;

		case SCR_CLOCK_MENU_STOPWATCH:
			SCREEN_TRAN(scr_clock_stopwatch_handle, &scr_clock_stopwatch);
			break;

		case SCR_CLOCK_MENU_TIMER:
			SCREEN_TRAN(scr_clock_timer_handle, &scr_clock_timer);
			break;

		case SCR_CLOCK_MENU_WORLD_CLOCK:
			SCREEN_TRAN(scr_world_clock_handle, &scr_world_clock);
			break;
		case SCR_CLOCK_MENU_CALENDAR:
			SCREEN_TRAN(scr_clock_calender_handle, &scr_clock_calender);
			break;
		case SCR_CLOCK_MENU_SETTING:
			SCREEN_TRAN(scr_clock_setting_handle, &scr_clock_setting);
			break;

		case SCR_CLOCK_MENU_EXIT:
			SCREEN_TRAN(scr_idle_handle, &scr_idle);
			break;

		default:
			APP_DBG("[MENU] item %u is not ready\n", current_location);
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");

		if (animation_running == 0)
		{
			if (current_location == 0)
				target_location = SCR_CLOCK_MENU_ITEM_NUMBER - 1;
			else
				target_location = current_location - 1;

			animation_direction = 1;
			animation_offset = 0;
			animation_running = 1;
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;
	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");

		if (animation_running == 0)
		{
			target_location = current_location + 1;

			if (target_location >= SCR_CLOCK_MENU_ITEM_NUMBER)
				target_location = 0;

			animation_direction = -1;
			animation_offset = 0;
			animation_running = 1;
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}
