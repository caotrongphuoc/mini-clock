#include "scr_clock_ringing.h"

#include "scr_clock_main.h"
#include "scr_clock_setting_display.h"

/*****************************************************************************/
/* Variable Declaration - Clock ringing */
/*****************************************************************************/
static uint8_t ringing_blink;

/*****************************************************************************/
/* View - Clock ringing */
/*****************************************************************************/
static void view_scr_clock_ringing();

view_dynamic_t dyn_view_scr_clock_ringing = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_ringing};

view_screen_t scr_clock_ringing = {
    &dyn_view_scr_clock_ringing,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

static void scr_clock_ringing_print_center(const char* text, uint8_t y, uint8_t size)
{
	uint8_t len = 0;

	while (text[len])
	{
		len++;
	}

	view_render.setTextSize(size);
	view_render.setCursor((LCD_WIDTH - len * 6 * size) / 2, y);
	view_render.print(text);
}

void view_scr_clock_ringing()
{
	mc_clock_time_state_t state;
	char time_text[9];

	mc_clock_time_get_state(&state);
	scr_clock_main_format_time(time_text, &state.time, scr_clock_setting_is_12h_format());

	view_render.clear();

	/* Blink the title bar once per second */
	if (ringing_blink)
	{
		view_render.fillRect(0, 0, LCD_WIDTH, 24, WHITE);
		view_render.setTextColor(BLACK);
	}
	else
	{
		view_render.drawRect(0, 0, LCD_WIDTH, 24, WHITE);
		view_render.setTextColor(WHITE);
	}
	scr_clock_ringing_print_center("ALARM", SCR_CLOCK_RINGING_TITLE_Y - 3, 2);

	view_render.setTextColor(WHITE);
	scr_clock_ringing_print_center(time_text, SCR_CLOCK_RINGING_TIME_Y, 2);
	scr_clock_ringing_print_center("Press any key", SCR_CLOCK_RINGING_HINT_Y, 1);
}

/*****************************************************************************/
/* Handle - Clock ringing */
/*****************************************************************************/
void scr_clock_ringing_enter(void)
{
	/* The interrupted screen does not get to clean up, so cancel every
	 * periodic display timer a clock screen may own. */
	timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
	timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_STOPWATCH_HELP);
	timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_MENU_ANIM_TICK);
	timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_WORLD_ANIM_TICK);
	timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE);
	timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_LOGO);

	SCREEN_TRAN(scr_clock_ringing_handle, &scr_clock_ringing);
}

static void scr_clock_ringing_leave(void)
{
	timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_ALARM_RING_TICK);
	SCREEN_TRAN(scr_clock_main_handle, &scr_clock_main);
}

void scr_clock_ringing_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY [ringing]\n");
		ringing_blink = 1;
		task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_UPDATE);
		timer_set(AC_TASK_DISPLAY_ID,
		          AC_DISPLAY_ALARM_RING_TICK,
		          AC_DISPLAY_ALARM_RING_TICK_INTERVAL,
		          TIMER_PERIODIC);
	}
	break;

	case AC_DISPLAY_ALARM_RING_TICK:
	{
		mc_clock_alarm_state_t state;

		mc_clock_alarm_get_state(&state);

		/* Auto-stopped by the alarm task after its timeout */
		if (!state.ringing)
		{
			scr_clock_ringing_leave();
			break;
		}

		ringing_blink = !ringing_blink;
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_RING_TICK);
		task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	case AC_DISPLAY_BUTON_UP_PRESSED:
	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
	case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
	case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
	case AC_DISPLAY_BUTON_MODE_UP_PRESSED:
	case AC_DISPLAY_BUTON_MODE_DOWN_PRESSED:
	{
		APP_DBG_SIG("dismiss [ringing]\n");
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DISMISS);
		scr_clock_ringing_leave();
	}
	break;

	default:
		break;
	}
}
