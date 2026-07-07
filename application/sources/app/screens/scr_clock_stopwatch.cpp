#include "scr_clock_stopwatch.h"

#include "mc_clock_stopwatch.h"

/*****************************************************************************/
/* View - Clock stopwatch */
/*****************************************************************************/

static void view_scr_clock_stopwatch();

view_dynamic_t dyn_view_scr_clock_stopwatch = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_stopwatch};

view_screen_t scr_clock_stopwatch = {
    &dyn_view_scr_clock_stopwatch,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void view_scr_clock_stopwatch()
{
	mc_clock_stopwatch_state_t stopwatch_state;
	char time_str[12];

	mc_clock_stopwatch_get_state(&stopwatch_state);

	uint32_t total_seconds = stopwatch_state.elapsed_ms / 1000;
	uint32_t minutes = total_seconds / 60;
	uint32_t seconds = total_seconds % 60;
	uint32_t ms = (stopwatch_state.elapsed_ms % 1000) / 10;

	sprintf(time_str, "%02lu:%02lu:%02lu", minutes, seconds, ms);

	view_render.clear();
	view_render.drawRoundRect(0, 0, 128, 64, 5, WHITE);
	view_render.setTextColor(WHITE);

	view_render.setCursor(10, 2);
	view_render.setTextSize(2);
	view_render.print("STOPWATCH");

	view_render.setCursor(20, 25);
	view_render.setTextSize(1);
	view_render.print("State: ");
	view_render.setCursor(70, 25);
	view_render.print(stopwatch_state.running ? "RUNNING" : "STOPPED");

	view_render.setCursor(12, 42);
	view_render.setTextSize(2);
	view_render.print(time_str);
}

/*****************************************************************************/
/* Handle - Clock stopwatch */
/*****************************************************************************/

void scr_clock_stopwatch_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		task_post_pure_msg(MC_CLOCK_STOPWATCH_ID, MC_CLOCK_STOPWATCH_SETUP);
		timer_set(AC_TASK_DISPLAY_ID,
		          MC_CLOCK_STOPWATCH_TICK,
		          MC_CLOCK_STOPWATCH_TICK_INTERVAL,
		          TIMER_PERIODIC);
	}
	break;

	case SCREEN_EXIT:
	{
		APP_DBG_SIG("SCREEN_EXIT\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_STOPWATCH_TICK);
	}
	break;

	case MC_CLOCK_STOPWATCH_TICK:
	{
		APP_DBG_SIG("MC_CLOCK_STOPWATCH_TICK\n");
		task_post_pure_msg(MC_CLOCK_STOPWATCH_ID, MC_CLOCK_STOPWATCH_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_STOPWATCH_ID, MC_CLOCK_STOPWATCH_START_PAUSE);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_MODE_PRESSED\n");
		task_post_pure_msg(MC_CLOCK_STOPWATCH_ID, MC_CLOCK_STOPWATCH_RESET);
		BUZZER_PlaySound(BUZZER_SOUND_USB_DISCONNECTED);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
	case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_%s_PRESSED\n",
		            msg->sig == AC_DISPLAY_BUTON_LONG_UP_PRESSED ? "UP" : "DOWN");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_STOPWATCH_TICK);
		SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
		BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
	}
	break;

	default:
		break;
	}
}
