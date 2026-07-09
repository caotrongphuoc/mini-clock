#include "scr_clock_setting_display.h"

#include "scr_clock_setting.h"

static uint8_t setting_display_location_choose;
static uint8_t setting_time_format_12h;
static uint8_t setting_color_invert;

static const char* const setting_display_item_name[SCR_CLOCK_SETTING_DISPLAY_ITEM_NUMBER] = {
    "Format",
    "Color",
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

	for (uint8_t i = 0; i < SCR_CLOCK_SETTING_DISPLAY_ITEM_NUMBER; i++)
	{
		uint8_t frame_y = SCR_CLOCK_SETTING_FRAME_Y_1 + SCR_CLOCK_SETTING_FRAME_STEP * i;
		bool selected = (i == setting_display_location_choose);
		uint8_t fg = selected ? BLACK : WHITE;

		if (selected)
		{
			view_render.fillRoundRect(SCR_CLOCK_SETTING_FRAME_X, frame_y,
			                          SCR_CLOCK_SETTING_FRAME_W, SCR_CLOCK_SETTING_FRAME_H,
			                          SCR_CLOCK_SETTING_FRAME_R, WHITE);
		}
		else
		{
			view_render.drawRoundRect(SCR_CLOCK_SETTING_FRAME_X, frame_y,
			                          SCR_CLOCK_SETTING_FRAME_W, SCR_CLOCK_SETTING_FRAME_H,
			                          SCR_CLOCK_SETTING_FRAME_R, WHITE);
		}

		view_render.setTextColor(fg);
		view_render.setCursor(8, frame_y + 2);
		view_render.print(setting_display_item_name[i]);

		if (i == SCR_CLOCK_SETTING_DISPLAY_FORMAT)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(setting_time_format_12h ? "[12]" : "[24]");
		}
		else if (i == SCR_CLOCK_SETTING_DISPLAY_COLOR)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(setting_color_invert ? "[INV]" : "[NOR]");
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
		setting_display_location_choose = SCR_CLOCK_SETTING_DISPLAY_FORMAT;
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		switch (setting_display_location_choose)
		{
		case SCR_CLOCK_SETTING_DISPLAY_FORMAT:
			setting_time_format_12h = !setting_time_format_12h;
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SCR_CLOCK_SETTING_DISPLAY_COLOR:
			setting_color_invert = !setting_color_invert;
			view_render.invertDisplay(setting_color_invert);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SCR_CLOCK_SETTING_DISPLAY_BACK:
		default:
			SCREEN_BACK();
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		if (setting_display_location_choose == 0)
		{
			setting_display_location_choose = SCR_CLOCK_SETTING_DISPLAY_ITEM_NUMBER - 1;
		}
		else
		{
			setting_display_location_choose--;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		setting_display_location_choose++;
		if (setting_display_location_choose >= SCR_CLOCK_SETTING_DISPLAY_ITEM_NUMBER)
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
	return setting_time_format_12h;
}
