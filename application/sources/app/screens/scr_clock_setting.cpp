#include "scr_clock_setting.h"

/*****************************************************************************/
/* Variable Declaration - Clock setting */
/*****************************************************************************/

static uint8_t setting_location_choose;
static uint8_t setting_color_invert;
static uint8_t setting_layout_mode;
static uint8_t setting_sound_off;

static const char* const setting_item_name[SCR_CLOCK_SETTING_ITEM_NUMBER] = {
    "Time",
    "Color",
    "Layout",
    "Sound",
    "Exit",
};

/*****************************************************************************/
/* View - Clock setting */
/*****************************************************************************/

static void view_scr_clock_setting();

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

void view_scr_clock_setting()
{
	view_render.setTextSize(1);

	for (uint8_t i = 0; i < SCR_CLOCK_SETTING_ITEM_NUMBER; i++)
	{
		uint8_t frame_y = SCR_CLOCK_SETTING_FRAME_Y_1 + SCR_CLOCK_SETTING_FRAME_STEP * i;
		bool selected = (i == setting_location_choose);
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
		view_render.print(setting_item_name[i]);

		if (i == SCR_CLOCK_SETTING_COLOR)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(setting_color_invert ? "[INV]" : "[NOR]");
		}
		else if (i == SCR_CLOCK_SETTING_LAYOUT)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(setting_layout_mode ? "[DTW]" : "[WTD]");
		}
		else if (i == SCR_CLOCK_SETTING_SOUND)
		{
			view_render.drawBitmap(110,
			                       frame_y + 2,
			                       setting_sound_off ? bitmap_clock_setting_speaker_off : bitmap_clock_setting_speaker_on,
			                       8,
			                       7,
			                       fg);
		}
	}

	view_render.setTextColor(WHITE);
}

/*****************************************************************************/
/* Handle - Clock setting */
/*****************************************************************************/

void scr_clock_setting_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		setting_location_choose = SCR_CLOCK_SETTING_TIME;
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		switch (setting_location_choose)
		{
		case SCR_CLOCK_SETTING_TIME:
			SCREEN_TRAN(scr_clock_time_setting_handle, &scr_clock_time_setting);
			break;

		case SCR_CLOCK_SETTING_COLOR:
			setting_color_invert = !setting_color_invert;
			view_render.invertDisplay(setting_color_invert);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SCR_CLOCK_SETTING_LAYOUT:
			setting_layout_mode = !setting_layout_mode;
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SCR_CLOCK_SETTING_SOUND:
			setting_sound_off = !setting_sound_off;
			BUZZER_Silent(setting_sound_off ? BUZZER_SILENT_ON : BUZZER_SILENT_OFF);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SCR_CLOCK_SETTING_EXIT:
		default:
			SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		if (setting_location_choose == 0)
		{
			setting_location_choose = SCR_CLOCK_SETTING_ITEM_NUMBER - 1;
		}
		else
		{
			setting_location_choose--;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		setting_location_choose++;
		if (setting_location_choose >= SCR_CLOCK_SETTING_ITEM_NUMBER)
		{
			setting_location_choose = 0;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}
