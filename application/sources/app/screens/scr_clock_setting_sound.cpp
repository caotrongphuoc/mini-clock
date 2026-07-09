#include "scr_clock_setting_sound.h"

#include "scr_clock_setting.h"
#include "mc_clock_time.h"

static uint8_t setting_sound_location_choose;
static uint8_t setting_sound_off;
static uint8_t setting_chime_enabled;

static const char* const setting_sound_item_name[SCR_CLOCK_SETTING_SOUND_ITEM_NUMBER] = {
    "Mute",
    "Chime",
    "Back",
};

static void view_scr_clock_setting_sound();

view_dynamic_t dyn_view_scr_clock_setting_sound = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_setting_sound};

view_screen_t scr_clock_setting_sound = {
    &dyn_view_scr_clock_setting_sound,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

void view_scr_clock_setting_sound()
{
	view_render.setTextSize(1);

	for (uint8_t i = 0; i < SCR_CLOCK_SETTING_SOUND_ITEM_NUMBER; i++)
	{
		uint8_t frame_y = SCR_CLOCK_SETTING_FRAME_Y_1 + SCR_CLOCK_SETTING_FRAME_STEP * i;
		bool selected = (i == setting_sound_location_choose);
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
		view_render.print(setting_sound_item_name[i]);

		if (i == SCR_CLOCK_SETTING_SOUND_MUTE)
		{
			view_render.drawBitmap(110,
			                       frame_y + 2,
			                       setting_sound_off ? bitmap_clock_setting_speaker_off : bitmap_clock_setting_speaker_on,
			                       8,
			                       7,
			                       fg);
		}
		else if (i == SCR_CLOCK_SETTING_SOUND_CHIME)
		{
			view_render.setCursor(92, frame_y + 2);
			view_render.print(setting_chime_enabled ? "[ON] " : "[OFF]");
		}
	}

	view_render.setTextColor(WHITE);
}

void scr_clock_setting_sound_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		setting_sound_location_choose = 0;
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		switch (setting_sound_location_choose)
		{
		case SCR_CLOCK_SETTING_SOUND_MUTE:
			setting_sound_off = !setting_sound_off;
			BUZZER_Silent(setting_sound_off ? BUZZER_SILENT_ON : BUZZER_SILENT_OFF);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SCR_CLOCK_SETTING_SOUND_CHIME:
			setting_chime_enabled = !setting_chime_enabled;
			mc_clock_time_set_chime_enabled(setting_chime_enabled);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SCR_CLOCK_SETTING_SOUND_BACK:
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
		if (setting_sound_location_choose == 0)
		{
			setting_sound_location_choose = SCR_CLOCK_SETTING_SOUND_ITEM_NUMBER - 1;
		}
		else
		{
			setting_sound_location_choose--;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		setting_sound_location_choose++;
		if (setting_sound_location_choose >= SCR_CLOCK_SETTING_SOUND_ITEM_NUMBER)
		{
			setting_sound_location_choose = 0;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}

void scr_clock_setting_sound_reset(void)
{
	setting_sound_off = 0;
	setting_chime_enabled = 0;
	BUZZER_Silent(BUZZER_SILENT_OFF);
	mc_clock_time_set_chime_enabled(0);
}
