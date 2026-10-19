#include "scr_clock_setting_display.h"

// Number of brightness levels
#define SETTING_DISPLAY_BRIGHT_LEVEL_NUMBER (3)

static uint8_t setting_display_location_choose;

static const uint8_t setting_bright_value[SETTING_DISPLAY_BRIGHT_LEVEL_NUMBER] = {
    0x20,
    0x80,
    0xCF,
};

static const char* const setting_bright_label[SETTING_DISPLAY_BRIGHT_LEVEL_NUMBER] = {
    "[LO]",
    "[MD]",
    "[HI]",
};

static const char* const setting_display_item_name[SETTING_DISPLAY_ITEM_NUMBER] = {
    "Format",
    "Color",
    "Bright",
    "Back",
};

static void view_scr_clock_setting_display();

view_dynamic_t dyn_view_scr_clock_setting_display = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_setting_display};

view_screen_t scr_clock_setting_display = {
    &dyn_view_scr_clock_setting_display,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

void view_scr_clock_setting_display()
{
	view_render.setTextSize(1);

	for (uint8_t i = 0; i < SETTING_DISPLAY_ITEM_NUMBER; i++)
	{
		uint8_t frame_y = SETTING_FRAME_Y_1 + SETTING_FRAME_STEP * i;
		bool selected = (i == setting_display_location_choose);
		uint8_t fg = selected ? BLACK : WHITE;

		if (selected)
		{
			view_render.fillRoundRect(SETTING_FRAME_X, frame_y,
			                          SETTING_FRAME_W, SETTING_FRAME_H,
			                          SETTING_FRAME_R, WHITE);
		}
		else
		{
			view_render.drawRoundRect(SETTING_FRAME_X, frame_y,
			                          SETTING_FRAME_W, SETTING_FRAME_H,
			                          SETTING_FRAME_R, WHITE);
		}

		view_render.setTextColor(fg);
		view_render.setCursor(8, frame_y + 2);
		view_render.print(setting_display_item_name[i]);

		if (i == SETTING_DISPLAY_FORMAT)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(clock_setting_data.format_12h ? "[12]" : "[24]");
		}
		else if (i == SETTING_DISPLAY_COLOR)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(clock_setting_data.color_invert ? "[INV]" : "[NOR]");
		}
		else if (i == SETTING_DISPLAY_BRIGHT)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(setting_bright_label[clock_setting_data.bright_level]);
		}
	}

	view_render.setTextColor(WHITE);
}

void scr_clock_setting_display_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		setting_display_location_choose = SETTING_DISPLAY_FORMAT;
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		switch (setting_display_location_choose)
		{
		case SETTING_DISPLAY_FORMAT:
			clock_setting_data.format_12h = !clock_setting_data.format_12h;
			mc_clock_setting_write(&clock_setting_data);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SETTING_DISPLAY_COLOR:
			clock_setting_data.color_invert = !clock_setting_data.color_invert;
			view_render.invertDisplay(clock_setting_data.color_invert);
			mc_clock_setting_write(&clock_setting_data);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SETTING_DISPLAY_BRIGHT:
			clock_setting_data.bright_level = (clock_setting_data.bright_level + 1) % SETTING_DISPLAY_BRIGHT_LEVEL_NUMBER;
			view_render.setContrast(setting_bright_value[clock_setting_data.bright_level]);
			mc_clock_setting_write(&clock_setting_data);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SETTING_DISPLAY_BACK:
		default:
			SCREEN_BACK();
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED\n");
		if (setting_display_location_choose == 0)
		{
			setting_display_location_choose = SETTING_DISPLAY_ITEM_NUMBER - 1;
		}
		else
		{
			setting_display_location_choose--;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED\n");
		setting_display_location_choose++;
		if (setting_display_location_choose >= SETTING_DISPLAY_ITEM_NUMBER)
		{
			setting_display_location_choose = 0;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}

uint8_t scr_clock_setting_is_12h_format(void)
{
	return clock_setting_data.format_12h;
}

void scr_clock_setting_display_reset(void)
{
	clock_setting_data.format_12h = 0;
	clock_setting_data.color_invert = 0;
	clock_setting_data.bright_level = SETTING_DISPLAY_BRIGHT_LEVEL_NUMBER - 1;
	view_render.invertDisplay(0);
	view_render.setContrast(setting_bright_value[clock_setting_data.bright_level]);
}

void scr_clock_setting_display_apply(void)
{
	if (clock_setting_data.bright_level >= SETTING_DISPLAY_BRIGHT_LEVEL_NUMBER)
	{
		clock_setting_data.bright_level = SETTING_DISPLAY_BRIGHT_LEVEL_NUMBER - 1;
	}
	view_render.setContrast(setting_bright_value[clock_setting_data.bright_level]);
	view_render.invertDisplay(clock_setting_data.color_invert);
}
