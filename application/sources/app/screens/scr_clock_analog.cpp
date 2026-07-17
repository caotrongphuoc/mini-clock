#include "scr_clock_analog.h"

#include <math.h>

#include "mc_clock_time.h"
#include "scr_clock_main.h"

#define ANALOG_CX 64
#define ANALOG_CY 38
#define ANALOG_R 22
#define ANALOG_TICK_MAJOR 5
#define ANALOG_TICK_MINOR 3

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

void scr_clock_analog_draw_header(const rtc_date_t* date)
{
	char buf[14];
	const char* wd = scr_clock_main_weekday_short_text(date->weekday);

	buf[0] = wd[0];
	buf[1] = wd[1];
	buf[2] = ' ';
	buf[3] = '0' + (date->date / 10) % 10;
	buf[4] = '0' + date->date % 10;
	buf[5] = '/';
	buf[6] = '0' + (date->month / 10) % 10;
	buf[7] = '0' + date->month % 10;
	buf[8] = '/';
	buf[9] = '0' + (date->year / 1000) % 10;
	buf[10] = '0' + (date->year / 100) % 10;
	buf[11] = '0' + (date->year / 10) % 10;
	buf[12] = '0' + date->year % 10;
	buf[13] = '\0';

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(25, 3);
	view_render.print(buf);
}

void scr_clock_analog_draw_face()
{
	view_render.drawCircle(ANALOG_CX, ANALOG_CY, ANALOG_R, WHITE);

	for (uint8_t h = 0; h < 12; h++)
	{
		float angle = h * 30.0f * DEG_TO_RAD;
		float s = sinf(angle);
		float c = cosf(angle);

		uint8_t tick_len = (h % 3 == 0) ? ANALOG_TICK_MAJOR : ANALOG_TICK_MINOR;

		int16_t x1 = ANALOG_CX + (int16_t)(s * ANALOG_R);
		int16_t y1 = ANALOG_CY - (int16_t)(c * ANALOG_R);
		int16_t x2 = ANALOG_CX + (int16_t)(s * (ANALOG_R - tick_len));
		int16_t y2 = ANALOG_CY - (int16_t)(c * (ANALOG_R - tick_len));

		view_render.drawLine(x1, y1, x2, y2, WHITE);
	}
}

void view_scr_clock_analog()
{
	mc_clock_time_state_t state;
	mc_clock_time_get_state(&state);

	view_render.clear();
	view_render.drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);

	scr_clock_analog_draw_header(&state.date);
	scr_clock_analog_draw_face();
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
