#include "scr_clock_alarm.h"

#include "mc_clock_alarm.h"

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

void view_scr_clock_alarm()
{
	mc_clock_alarm_state_t alarm_state;
	char line[22];

	mc_clock_alarm_get_state(&alarm_state);

	view_render.clear();

	if (alarm_state.ringing)
	{
		view_render.fillRect(0, 0, 128, 64, WHITE);
		view_render.setTextColor(BLACK);

		view_render.setCursor(22, 8);
		view_render.setTextSize(2);
		view_render.print("ALARM");

		view_render.setCursor(24, 32);
		view_render.setTextSize(1);
		view_render.print("MODE to stop");
		return;
	}

	if (alarm_state.editing)
	{
		mc_clock_alarm_item_t* alarm = &alarm_state.alarm[alarm_state.editing_alarm];

		view_render.drawRoundRect(0, 0, 128, 64, 5, WHITE);
		view_render.setTextColor(WHITE);

		view_render.setCursor(5, 5);
		view_render.setTextSize(2);
		view_render.print("Edit Alarm");

		sprintf(line, "%02u:%02u", alarm->hour, alarm->minute);
		view_render.setCursor(15, 25);
		view_render.setTextSize(3);
		view_render.print(line);

		if (alarm_state.editing_field == 0)
		{
			view_render.drawRect(14, 24, 38, 25, WHITE);
		}
		else
		{
			view_render.drawRect(66, 24, 38, 25, WHITE);
		}

		view_render.setCursor(10, 53);
		view_render.setTextSize(1);
		view_render.print("MODE save");
		return;
	}

	for (uint8_t i = 0; i < 3; i++)
	{
		uint8_t alarm_index = alarm_state.scroll_offset + i;
		if (alarm_index >= alarm_state.total_alarm)
		{
			break;
		}

		uint8_t y = i * 13;
		uint16_t bg = (alarm_index == alarm_state.current_item) ? WHITE : BLACK;
		uint16_t fg = (alarm_index == alarm_state.current_item) ? BLACK : WHITE;

		view_render.fillRect(0, y, 128, 13, bg);
		view_render.setTextSize(1);
		view_render.setTextColor(fg);

		sprintf(line,
		        "%u. %02u:%02u %s",
		        alarm_index + 1,
		        alarm_state.alarm[alarm_index].hour,
		        alarm_state.alarm[alarm_index].minute,
		        alarm_state.alarm[alarm_index].enabled ? "[ON]" : "[OFF]");

		view_render.setCursor(4, y + 3);
		view_render.print(line);
	}

	view_render.fillRect(0, 40, 128, 24, BLACK);

	view_render.fillRoundRect(4,
	                          43,
	                          70,
	                          14,
	                          3,
	                          alarm_state.current_item == alarm_state.total_alarm ? WHITE : BLACK);
	view_render.setTextColor(alarm_state.current_item == alarm_state.total_alarm ? BLACK : WHITE);
	view_render.setCursor(10, 47);
	view_render.print("Add New");

	view_render.fillRoundRect(80,
	                          43,
	                          45,
	                          14,
	                          3,
	                          alarm_state.current_item == alarm_state.total_alarm + 1 ? WHITE : BLACK);
	view_render.setTextColor(alarm_state.current_item == alarm_state.total_alarm + 1 ? BLACK : WHITE);
	view_render.setCursor(88, 47);
	view_render.print("Back");
}

/*****************************************************************************/
/* Handle - Clock alarm */
/*****************************************************************************/

void scr_clock_alarm_handle(ak_msg_t* msg)
{
	mc_clock_alarm_state_t alarm_state;

	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_SETUP);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		mc_clock_alarm_get_state(&alarm_state);
		if (alarm_state.ringing)
		{
			task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DISMISS);
			break;
		}

		if (alarm_state.editing == 0 &&
		    alarm_state.current_item == alarm_state.total_alarm + 1)
		{
			SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
			break;
		}

		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_SELECT);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_MODE_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_NEXT_FIELD);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		mc_clock_alarm_get_state(&alarm_state);
		if (alarm_state.ringing)
		{
			task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DISMISS);
			break;
		}

		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_PREV);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		mc_clock_alarm_get_state(&alarm_state);
		if (alarm_state.ringing)
		{
			task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DISMISS);
			break;
		}

		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_NEXT);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
	case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_%s_PRESSED\n",
		            msg->sig == AC_DISPLAY_BUTON_LONG_UP_PRESSED ? "UP" : "DOWN");
		SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
		BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
	}
	break;

	default:
		break;
	}
}
