#include "scr_clock_setting.h"

/*****************************************************************************/
/* Variable Declaration - Clock setting */
/*****************************************************************************/

static uint8_t setting_location_choose;

static const char* const setting_item_name[SETTING_ITEM_NUMBER] = {
    "Time",
    "Display",
    "Sound",
    "Reset",
    "Exit",
};
static const unsigned char* const setting_bitmap[SETTING_ITEM_NUMBER] =
    {
        bitmap_clock_4,
        bitmap_monitor,
        bitmap_volume_2,
        bitmap_rotate_ccw,
        bitmap_log_out,
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

	for (uint8_t i = 0; i < SETTING_ITEM_NUMBER; i++)
	{
		uint8_t frame_y = SETTING_FRAME_Y_1 +
		                  SETTING_FRAME_STEP * i;

		bool selected = (i == setting_location_choose);

		uint8_t fg = selected ? BLACK : WHITE;

		if (selected)
		{
			view_render.fillRoundRect(
			    SETTING_FRAME_X,
			    frame_y,
			    SETTING_FRAME_W,
			    SETTING_FRAME_H,
			    SETTING_FRAME_R,
			    WHITE);
		}
		else
		{
			view_render.drawRoundRect(
			    SETTING_FRAME_X,
			    frame_y,
			    SETTING_FRAME_W,
			    SETTING_FRAME_H,
			    SETTING_FRAME_R,
			    WHITE);
		}

		// Text left
		view_render.setTextColor(fg);
		view_render.setCursor(8, frame_y + 2);
		view_render.print(setting_item_name[i]);

		// Bitmap right
		view_render.drawBitmap(
		    105,
		    frame_y + 1,
		    setting_bitmap[i],
		    11,
		    11,
		    fg);
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
		setting_location_choose = SETTING_TIME;
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		switch (setting_location_choose)
		{
		case SETTING_TIME:
			SCREEN_TRAN(scr_clock_time_setting_handle, &scr_clock_time_setting);
			break;

		case SETTING_DISPLAY:
			SCREEN_TRAN(scr_clock_setting_display_handle, &scr_clock_setting_display);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SETTING_SOUND:
			SCREEN_TRAN(scr_clock_setting_sound_handle, &scr_clock_setting_sound);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;

		case SETTING_RESET:
			scr_clock_setting_display_reset();
			scr_clock_setting_sound_reset();
			clock_setting_data.world_country = 0;
			mc_clock_world_clock_select_country(0);
			mc_clock_setting_write(&clock_setting_data);
			task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_REARM);
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
			break;

		case SETTING_EXIT:
		default:
			SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
			break;
		}
	}
	break;

	case AC_DISPLAY_BUTTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED\n");
		if (setting_location_choose > 0)
		{
			setting_location_choose--;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED\n");
		if (setting_location_choose < SETTING_ITEM_NUMBER - 1)
		{
			setting_location_choose++;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}
