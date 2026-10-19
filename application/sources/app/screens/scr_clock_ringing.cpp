#include "scr_clock_ringing.h"

/*****************************************************************************/
/* Variable Declaration - Clock ringing */
/*****************************************************************************/
static uint8_t ringing_source = RING_SOURCE_MAX; /* MAX = not ringing */
static uint8_t ringing_sound;
static uint8_t ringing_elapsed_s;
static uint8_t ringing_blink;

static const char* const ringing_title[RING_SOURCE_MAX] = {
    "ALARM",
    "TIMER",
    "EVENT",
};

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
	char time_text[9];

	scr_clock_main_format_time(time_text, &mc_clock_time.time, scr_clock_setting_is_12h_format());

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

	if (ringing_source < RING_SOURCE_MAX)
	{
		scr_clock_ringing_print_center(ringing_title[ringing_source], RINGING_TITLE_Y, 2);
	}

	view_render.setTextColor(WHITE);
	scr_clock_ringing_print_center(time_text, RINGING_TIME_Y, 2);
	scr_clock_ringing_print_center("Press any key", RINGING_HINT_Y, 1);
}

/*****************************************************************************/
/* Private helpers - Clock ringing */
/*****************************************************************************/
/* Tell the source object to stop ringing (it silences the buzzer and re-arms) */
static void scr_clock_ringing_dismiss_source(void)
{
	switch (ringing_source)
	{
	case RING_SOURCE_ALARM:
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_DISMISS);
		break;

	case RING_SOURCE_TIMER:
		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_DISMISS);
		break;

	case RING_SOURCE_CALENDAR:
		task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_ALARM_DISMISS);
		break;

	default:
		break;
	}

	ringing_source = RING_SOURCE_MAX;
}

static void scr_clock_ringing_leave(void)
{
	scr_clock_ringing_dismiss_source();
	timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_ALARM_RING_TICK);
	SCREEN_TRAN(scr_clock_main_handle, &scr_clock_main);
}

/*****************************************************************************/
/* Handle - Clock ringing */
/*****************************************************************************/
void scr_clock_ringing_enter(ak_msg_t* msg)
{
	if (get_data_len_common_msg(msg) != sizeof(mc_clock_ring_req_t))
	{
		return;
	}

	mc_clock_ring_req_t* req = (mc_clock_ring_req_t*)get_data_common_msg(msg);

	if (req->source >= RING_SOURCE_MAX)
	{
		return;
	}

	/* A second source pre-empts the first; the first one is dismissed so it
	 * does not stay latched in its ringing mc_clock_time. */
	scr_clock_ringing_dismiss_source();

	ringing_source = req->source;
	ringing_sound = req->sound;
	ringing_elapsed_s = 0;

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
		ringing_elapsed_s++;

		if (ringing_elapsed_s >= RING_TIMEOUT_S)
		{
			scr_clock_ringing_leave();
			break;
		}

		if ((ringing_elapsed_s % RING_REPEAT_S) == 0)
		{
			BUZZER_PlaySound((buzzer_sound_t)ringing_sound);
		}

		ringing_blink = !ringing_blink;
		task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED:
	case AC_DISPLAY_BUTTON_UP_PRESSED:
	case AC_DISPLAY_BUTTON_DOWN_PRESSED:
	case AC_DISPLAY_BUTTON_LONG_MODE_PRESSED:
	case AC_DISPLAY_BUTTON_LONG_UP_PRESSED:
	case AC_DISPLAY_BUTTON_LONG_DOWN_PRESSED:
	case AC_DISPLAY_BUTTON_MODE_UP_PRESSED:
	case AC_DISPLAY_BUTTON_MODE_DOWN_PRESSED:
	{
		APP_DBG_SIG("dismiss [ringing]\n");
		scr_clock_ringing_leave();
	}
	break;

	default:
		break;
	}
}
