#include "scr_clock_timer.h"
#include "mc_clock_timer.h"

static void view_scr_clock_timer();
static uint8_t timer_help = 0;
static uint8_t timer_help_counter = 0;

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

static void scr_clock_timer_draw_help()
{
	view_render.setTextColor(WHITE);

	view_render.setTextSize(2);
	view_render.setCursor(34, 3);
	view_render.print("TIMER");

	view_render.setTextSize(1);

	view_render.setCursor(15, 22);
	view_render.print("MODE  Select field");

	view_render.setCursor(15, 32);
	view_render.print("UP    Increase");

	view_render.setCursor(15, 42);
	view_render.print("DOWN  Decrease");

	view_render.setCursor(15, 52);
	view_render.print("LONG START/BACK");
}

void view_scr_clock_timer()
{
	mc_clock_timer_state_t timer_state;
	char time_buf[16];
	uint8_t progress = 0;

	mc_clock_timer_get_state(&timer_state);

	view_render.clear();
	if (timer_help)
	{
		scr_clock_timer_draw_help();
		return;
	}

	xsprintf(time_buf, "%02u:%02u:%02u",
	         timer_state.remaining_hour,
	         timer_state.remaining_min,
	         timer_state.remaining_sec);

	if (timer_state.set_min > 0)
	{
		uint32_t total =
		    timer_state.set_hour * 3600UL +
		    timer_state.set_min * 60UL +
		    timer_state.set_sec;

		uint32_t remain =
		    timer_state.remaining_hour * 3600UL +
		    timer_state.remaining_min * 60UL +
		    timer_state.remaining_sec;

		if (total > 0)
		{
			progress = ((total - remain) * 100UL) / total;
		}
	}

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

	view_render.setCursor(20, 26);
	view_render.setTextSize(2);
	view_render.print(time_buf);

	view_render.setCursor(5, 52);
	view_render.setTextSize(1);
	view_render.print(timer_state.running ? "RUNNING" : "STOPPED");
	view_render.setCursor(75, 52);

	switch (timer_state.edit_field)
	{
	case MC_TIMER_EDIT_HOUR:
		view_render.print("HOUR");
		break;

	case MC_TIMER_EDIT_MIN:
		view_render.print("MIN");
		break;

	case MC_TIMER_EDIT_SEC:
		view_render.print("SEC");
		break;
	}

	view_render.fillRect(5, 60, (progress * 118) / 100, 3, WHITE);

	if (!timer_state.running)
	{
		uint8_t x = 20;

		switch (timer_state.edit_field)
		{
		case MC_TIMER_EDIT_HOUR:
			x = 20;
			break;

		case MC_TIMER_EDIT_MIN:
			x = 56;
			break;

		case MC_TIMER_EDIT_SEC:
			x = 92;
			break;
		}

		view_render.drawLine(x, 54, x + 20, 54, WHITE);
	}
}

void scr_clock_timer_handle(ak_msg_t* msg)
{
	mc_clock_timer_state_t timer_state;

	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");

		timer_help = 1;
		timer_help_counter = 0;

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
		task_post_pure_msg(
		    MC_CLOCK_TIMER_ID,
		    MC_CLOCK_TIMER_UPDATE);

		if (timer_help)
		{
			timer_help_counter++;

			if (timer_help_counter >= (5000 / MC_CLOCK_TIMER_TICK_INTERVAL))
			{
				timer_help = 0;
			}
		}
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		timer_help = 0;
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		mc_clock_timer_get_state(&timer_state);

		if (timer_state.finished)
		{
			task_post_pure_msg(MC_CLOCK_TIMER_ID,
			                   MC_CLOCK_TIMER_DISMISS);
			break;
		}

		if (!timer_state.running)
		{
			task_post_pure_msg(MC_CLOCK_TIMER_ID,
			                   MC_CLOCK_TIMER_NEXT_FIELD);
		}
		else
		{
			task_post_pure_msg(MC_CLOCK_TIMER_ID,
			                   MC_CLOCK_TIMER_START_PAUSE);
		}

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	// This is to start the timer
	case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_MODE_PRESSED\n");
		timer_help = 0;
		task_post_pure_msg(MC_CLOCK_TIMER_ID,
		                   MC_CLOCK_TIMER_START_PAUSE);

		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_UP_PRESSED\n");
		timer_help = 0;
		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_RESET);
		BUZZER_PlaySound(BUZZER_SOUND_USB_DISCONNECTED);
	}
	break;

	case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_DOWN_PRESSED\n");
		timer_help = 0;
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIMER_TICK);
		SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
		BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		timer_help = 0;
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
		timer_help = 0;
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

		// case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
		// case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
		// {
		// 	APP_DBG_SIG("AC_DISPLAY_BUTON_LONG_%s_PRESSED\n",
		// 	            msg->sig == AC_DISPLAY_BUTON_LONG_UP_PRESSED ? "UP" : "DOWN");
		// 	timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIMER_TICK);
		// 	SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
		// 	BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
		// }
		// break;

	default:
		break;
	}
}
