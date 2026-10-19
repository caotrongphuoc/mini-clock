#include "scr_clock_alarm.h"
/*****************************************************************************/
/* View - Clock alarm */
/*****************************************************************************/

static void view_scr_clock_alarm();

view_dynamic_t dyn_view_scr_clock_alarm = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_alarm};

view_screen_t scr_clock_alarm = {
    &dyn_view_scr_clock_alarm,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

static void view_scr_clock_alarm_editing()
{
	const mc_clock_alarm_item_t* alarm =
	    &mc_clock_alarm.alarm[mc_clock_alarm.editing_alarm];

	char buf[16];

	view_render.drawRoundRect(0, 0, LCD_WIDTH, LCD_HEIGHT, 5, WHITE);
	view_render.setTextColor(WHITE);

	view_render.setCursor(5, 5);
	view_render.setTextSize(2);
	view_render.print("Edit Alarm");

	switch (mc_clock_alarm.editing_field)
	{
	case ALARM_EDIT_HOUR: // Hour
	{
		xsprintf(buf, "[%02u]:%02u %s",
		         alarm->hour,
		         alarm->minute,
		         alarm->enabled ? "ON" : "OFF");

		view_render.setCursor(5, 28);
		view_render.setTextSize(2);
		view_render.print(buf);

		view_render.setCursor(5, 55);
		view_render.setTextSize(1);
		view_render.print("Edit Hour");
	}
	break;

	case ALARM_EDIT_MINUTE: // Minute
	{
		xsprintf(buf, "%02u:[%02u] %s",
		         alarm->hour,
		         alarm->minute,
		         alarm->enabled ? "ON" : "OFF");

		view_render.setCursor(5, 28);
		view_render.setTextSize(2);
		view_render.print(buf);

		view_render.setCursor(5, 55);
		view_render.setTextSize(1);
		view_render.print("Edit Minute");
	}
	break;

	case ALARM_EDIT_ENABLE: // ON/OFF
	{
		xsprintf(buf, "%02u:%02u [%s]",
		         alarm->hour,
		         alarm->minute,
		         alarm->enabled ? "ON" : "OFF");

		view_render.setCursor(5, 28);
		view_render.setTextSize(2);
		view_render.print(buf);

		view_render.setCursor(5, 55);
		view_render.setTextSize(1);
		view_render.print("Edit Enable");
	}
	break;

	case ALARM_EDIT_DELETE: // Delete
	{
		view_render.setCursor(5, 28);
		view_render.setTextSize(2);
		view_render.print("Delete?");

		view_render.setCursor(5, 55);
		view_render.setTextSize(1);
		view_render.print("MODE = YES");
	}
	break;

	case ALARM_EDIT_SAVE: // Save
	{
		view_render.setCursor(5, 28);
		view_render.setTextSize(2);
		view_render.print("Save?");

		view_render.setCursor(5, 55);
		view_render.setTextSize(1);
		view_render.print("MODE = YES");
	}
	break;
	}
}

static void view_scr_clock_alarm_list()
{
	uint8_t visible_lines = ALARM_LIST_VISIBLE;

	for (uint8_t i = 0; i < visible_lines; i++)
	{
		uint8_t idx = mc_clock_alarm.scroll_offset + i;
		if (idx >= mc_clock_alarm.total_alarm)
		{
			break;
		}

		uint8_t y = i * 13;
		uint16_t bg = (idx == mc_clock_alarm.current_item) ? WHITE : BLACK;
		uint16_t fg = (idx == mc_clock_alarm.current_item) ? BLACK : WHITE;
		char line[16];

		view_render.fillRect(0, y, LCD_WIDTH, 13, bg);
		view_render.setTextColor(fg);
		view_render.setTextSize(1);
		xsprintf(line, "%u. %02u:%02u %s",
		         idx + 1,
		         mc_clock_alarm.alarm[idx].hour,
		         mc_clock_alarm.alarm[idx].minute,
		         mc_clock_alarm.alarm[idx].enabled ? "[ON]" : "[OFF]");
		view_render.setCursor(4, y + 3);
		view_render.print(line);
	}

	uint8_t add_focus = (mc_clock_alarm.current_item == mc_clock_alarm.total_alarm);
	uint8_t back_focus = (mc_clock_alarm.current_item == mc_clock_alarm.total_alarm + 1);

	view_render.fillRect(0, 40, LCD_WIDTH, 24, BLACK);

	view_render.fillRoundRect(4, 43, 70, 14, 3, add_focus ? WHITE : BLACK);
	view_render.drawRoundRect(4, 43, 70, 14, 3, WHITE);
	view_render.setTextColor(add_focus ? BLACK : WHITE);
	view_render.setCursor(10, 47);
	view_render.print("Add New");

	view_render.fillRoundRect(80, 43, 45, 14, 3, back_focus ? WHITE : BLACK);
	view_render.drawRoundRect(80, 43, 45, 14, 3, WHITE);
	view_render.setTextColor(back_focus ? BLACK : WHITE);
	view_render.setCursor(88, 47);
	view_render.print("Back");
}

void view_scr_clock_alarm()
{
	view_render.clear();

	if (mc_clock_alarm.editing)
	{
		view_scr_clock_alarm_editing();
	}
	else
	{
		view_scr_clock_alarm_list();
	}
}

/*****************************************************************************/
/* Handle - Clock alarm */
/*****************************************************************************/

void scr_clock_alarm_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");

		/* Start at the top of the list; an edit left unfinished is discarded */
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_SETUP);

		break;
	}

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");

		uint8_t add_item = mc_clock_alarm.total_alarm;
		uint8_t back_item = mc_clock_alarm.total_alarm + 1;

		if (!mc_clock_alarm.editing)
		{
			if (mc_clock_alarm.current_item == back_item)
			{
				SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
				BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
				break;
			}

			if (mc_clock_alarm.current_item == add_item)
			{
				task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_SELECT);
				BUZZER_PlaySound(BUZZER_SOUND_CLICK);
				break;
			}

			// Existing alarm selected
			task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_SELECT);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
			break;
		}

		// Editing mode
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_SELECT);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;
	case AC_DISPLAY_BUTTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED\n");

		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_UP);

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED\n");

		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DOWN);

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_LONG_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_LONG_MODE_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_NEXT_FIELD);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_UP_PRESSED\n");

		task_post_pure_msg(
		    MC_CLOCK_ALARM_ID,
		    MC_CLOCK_ALARM_ADD_NEW);

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_DOWN_PRESSED\n");

		SCREEN_TRAN(scr_clock_menu_handle,
		            &scr_clock_menu);

		BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
	}
	break;

	case AC_DISPLAY_BUTTON_LONG_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_LONG_UP_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_TOGGLE);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTTON_LONG_DOWN_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DELETE);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}
