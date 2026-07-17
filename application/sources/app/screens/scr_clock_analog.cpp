#include "scr_clock_analog.h"

#include "mc_clock_time.h"

static void view_scr_clock_analog();

view_dynamic_t dyn_view_scr_clock_analog = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_analog};

view_screen_t scr_clock_analog = {
    &dyn_view_scr_clock_analog,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void view_scr_clock_analog()
{
	view_render.clear();
	view_render.drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(40, 28);
	view_render.print("ANALOG");
}

void scr_clock_analog_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY [analog]\n");
		task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_SETUP);
		timer_set(AC_TASK_DISPLAY_ID,
		          MC_CLOCK_TIME_TICK,
		          MC_CLOCK_TIME_TICK_INTERVAL,
		          TIMER_PERIODIC);
	}
	break;

	case MC_CLOCK_TIME_TICK:
	{
		APP_DBG_SIG("MC_CLOCK_TIME_TICK [analog]\n");
		task_post_pure_msg(MC_CLOCK_TIME_ID, MC_CLOCK_TIME_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED [analog]\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
	}
	break;

	default:
		break;
	}
}
