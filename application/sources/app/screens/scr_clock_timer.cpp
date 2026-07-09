#include "scr_clock_timer.h"

#include "mc_clock_timer.h"

/*****************************************************************************/
/* View - Clock timer */
/*****************************************************************************/

static void view_scr_clock_timer();

view_dynamic_t dyn_view_scr_clock_timer = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_timer};

view_screen_t scr_clock_timer = {
    &dyn_view_scr_clock_timer,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void view_scr_clock_timer()
{
	mc_clock_timer_state_t timer_state;
	char time_buf[12];
	uint16_t elapsed_min;
	uint8_t progress = 0;

	mc_clock_timer_get_state(&timer_state);
	sprintf(time_buf, "%02u:%02u", timer_state.remaining_min, timer_state.remaining_sec);

	if (timer_state.set_minutes > 0)
	{
		elapsed_min = timer_state.set_minutes - timer_state.remaining_min;
		progress = (elapsed_min * 100) / timer_state.set_minutes;
	}

	view_render.clear();

	if (timer_state.finished)
	{
		view_render.fillRect(0, 0, 128, 64, WHITE);
		view_render.setTextColor(BLACK);

		view_render.setCursor(18, 8);
		view_render.setTextSize(2);
		view_render.print("TIME UP");

		view_render.setCursor(20, 34);
		view_render.setTextSize(1);
		view_render.print("MODE to stop");
		return;
	}

	view_render.drawRoundRect(0, 0, 128, 64, 6, WHITE);
	view_render.setTextColor(WHITE);

	view_render.setCursor(34, 3);
	view_render.setTextSize(2);
	view_render.print("TIMER");

	view_render.setCursor(18, 26);
	view_render.setTextSize(3);
	view_render.print(time_buf);

	view_render.setCursor(5, 52);
	view_render.setTextSize(1);
	view_render.print(timer_state.running ? "RUNNING" : "STOPPED");

	view_render.fillRect(5, 60, (progress * 118) / 100, 3, WHITE);
}

/*****************************************************************************/
/* Handle - Clock timer */
/*****************************************************************************/

void scr_clock_timer_handle(ak_msg_t* msg)
{
	mc_clock_timer_state_t timer_state;

	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_SETUP);
		timer_set(AC_TASK_DISPLAY_ID,
		          MC_CLOCK_TIMER_TICK,
		          MC_CLOCK_TIMER_TICK_INTERVAL,
		          TIMER_PERIODIC);
	}
	break;

	case MC_CLOCK_TIMER_TICK:
	{
		APP_DBG_SIG("MC_CLOCK_TIMER_TICK\n");
		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		mc_clock_timer_get_state(&timer_state);
		if (timer_state.finished)
		{
			task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_DISMISS);
			break;
		}

		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_START_PAUSE);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_MODE_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_RESET);
		BUZZER_PlaySound(BUZZER_SOUND_USB_DISCONNECTED);
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		mc_clock_timer_get_state(&timer_state);
		if (timer_state.finished)
		{
			task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_DISMISS);
			break;
		}

		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_INC);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		mc_clock_timer_get_state(&timer_state);
		if (timer_state.finished)
		{
			task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_DISMISS);
			break;
		}

		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_DEC);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
	case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_%s_PRESSED\n",
		            msg->sig == AC_DISPLAY_BUTON_LONG_UP_PRESSED ? "UP" : "DOWN");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIMER_TICK);
		SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
		BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
	}
	break;

	default:
		break;
	}
}
