#include "scr_clock_stopwatch.h"
#include "mc_clock_stopwatch.h"

static uint8_t stopwatch_lap_scroll = 0;
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

static void scr_clock_stopwatch_format_time(char* buf, uint32_t elapsed_ms)
{
	uint32_t total_sec = elapsed_ms / 1000;
	uint32_t minutes = total_sec / 60;
	uint32_t seconds = total_sec % 60;
	uint32_t centis = (elapsed_ms % 1000) / 10;

	xsprintf(buf, "%02lu:%02lu.%02lu", minutes, seconds, centis);
}

void view_scr_clock_stopwatch()
{
	mc_clock_stopwatch_state_t state;
	char time_buf[12];

	mc_clock_stopwatch_get_state(&state);
	scr_clock_stopwatch_format_time(time_buf, state.elapsed_ms);

	view_render.clear();
	view_render.drawRoundRect(0, 0, 128, 64, 5, WHITE);
	view_render.setTextColor(WHITE);

	view_render.setCursor(10, 3);
	view_render.setTextSize(2);
	view_render.print("STOPWATCH");

	view_render.setCursor(6, 24);
	view_render.setTextSize(2);
	view_render.print(time_buf);

	view_render.setCursor(6, 46);
	view_render.setTextSize(1);

	/* Status */
	view_render.setCursor(6, 42);
	view_render.print(state.running ? "RUN" : "STOP");

	/* Show laps */
	uint8_t start = stopwatch_lap_scroll;

	for (uint8_t i = 0; i < 2; i++)
	{
		uint8_t lap_index = start + i;

		if (lap_index >= state.lap_count)
		{
			break;
		}

		char lap_buf[20];

		uint32_t lap_time = state.lap_ms[lap_index];

		uint32_t sec = lap_time / 1000;
		uint32_t ms = (lap_time % 1000) / 10;

		xsprintf(lap_buf,
		         "LAP%u %02lu:%02lu.%02lu",
		         lap_index + 1,
		         sec / 60,
		         sec % 60,
		         ms);

		view_render.setCursor(35, 42 + i * 10);
		view_render.print(lap_buf);
	}
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

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		task_post_pure_msg(MC_CLOCK_STOPWATCH_ID, MC_CLOCK_STOPWATCH_LAP);

		stopwatch_lap_scroll = 0;

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");

		mc_clock_stopwatch_state_t state;
		mc_clock_stopwatch_get_state(&state);

		if (stopwatch_lap_scroll + 2 < state.lap_count)
		{
			stopwatch_lap_scroll++;
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
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
