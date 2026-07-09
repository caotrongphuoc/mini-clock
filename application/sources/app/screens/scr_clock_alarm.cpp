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

static void view_scr_clock_alarm_editing(const mc_clock_alarm_state_t* state)
{
	const mc_clock_alarm_item_t* alarm = &state->alarm[state->editing_alarm];
	char buf[24];

	view_render.drawRoundRect(0, 0, 128, 64, 5, WHITE);
	view_render.setTextColor(WHITE);

	view_render.setCursor(5, 5);
	view_render.setTextSize(2);
	view_render.print("Edit Alarm");

	sprintf(buf, "%02u:%02u %s",
	        alarm->hour, alarm->minute,
	        alarm->enabled ? "ON" : "OFF");
	view_render.setCursor(5, 28);
	view_render.setTextSize(2);
	view_render.print(buf);

	view_render.setCursor(5, 55);
	view_render.setTextSize(1);
	view_render.print(state->editing_field == 0 ? "Edit Hour" : "Edit Minute");
}

static void view_scr_clock_alarm_list(const mc_clock_alarm_state_t* state)
{
	uint8_t visible_lines = 3;

	for (uint8_t i = 0; i < visible_lines; i++)
	{
		uint8_t idx = state->scroll_offset + i;
		if (idx >= state->total_alarm)
		{
			break;
		}

		uint8_t y = i * 13;
		uint16_t bg = (idx == state->current_item) ? WHITE : BLACK;
		uint16_t fg = (idx == state->current_item) ? BLACK : WHITE;
		char line[25];

		view_render.fillRect(0, y, 128, 13, bg);
		view_render.setTextColor(fg);
		view_render.setTextSize(1);
		sprintf(line, "%u. %02u:%02u %s",
		        idx + 1,
		        state->alarm[idx].hour,
		        state->alarm[idx].minute,
		        state->alarm[idx].enabled ? "[ON]" : "[OFF]");
		view_render.setCursor(4, y + 3);
		view_render.print(line);
	}

	uint8_t add_focus = (state->current_item == state->total_alarm);
	uint8_t back_focus = (state->current_item == state->total_alarm + 1);

	view_render.fillRect(0, 40, 128, 24, BLACK);

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
	mc_clock_alarm_state_t state;

	mc_clock_alarm_get_state(&state);

	view_render.clear();

	if (state.editing)
	{
		view_scr_clock_alarm_editing(&state);
	}
	else
	{
		view_scr_clock_alarm_list(&state);
	}
}

/*****************************************************************************/
/* Handle - Clock alarm */
/*****************************************************************************/

void scr_clock_alarm_handle(ak_msg_t* msg)
{
	mc_clock_alarm_state_t state;

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
		mc_clock_alarm_get_state(&state);

		if (!state.editing && state.current_item == state.total_alarm + 1)
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
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_PREV);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_NEXT);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_UP_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_TOGGLE);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_DOWN_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DELETE);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	default:
		break;
	}
}
