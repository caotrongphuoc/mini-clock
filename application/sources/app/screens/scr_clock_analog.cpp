#include "scr_clock_analog.h"

#include <math.h>

#include "mc_clock_time.h"
#include "scr_clock_main.h"

#define ANALOG_CX 64
#define ANALOG_CY 38
#define ANALOG_R 22
#define ANALOG_TICK_MAJOR 5
#define ANALOG_TICK_MINOR 3
#define ANALOG_HOUR_LEN 10
#define ANALOG_MIN_LEN 15
#define ANALOG_SEC_LEN 19
#define ANALOG_HUB_R 2
#define ANALOG_NUM_R 13

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

void scr_clock_analog_draw_hand(float sin_a, float cos_a, uint8_t length, uint8_t thickness)
{
	int16_t x_end = ANALOG_CX + (int16_t)(sin_a * length);
	int16_t y_end = ANALOG_CY - (int16_t)(cos_a * length);

	view_render.drawLine(ANALOG_CX, ANALOG_CY, x_end, y_end, WHITE);

	if (thickness < 2)
	{
		return;
	}

	int8_t px, py;
	if (fabsf(cos_a) > fabsf(sin_a))
	{
		px = (cos_a > 0) ? 1 : -1;
		py = 0;
	}
	else
	{
		px = 0;
		py = (sin_a > 0) ? 1 : -1;
	}

	view_render.drawLine(ANALOG_CX + px, ANALOG_CY + py, x_end + px, y_end + py, WHITE);

	if (thickness >= 3)
	{
		view_render.drawLine(ANALOG_CX - px, ANALOG_CY - py, x_end - px, y_end - py, WHITE);
	}
}

void scr_clock_analog_draw_hands(const rtc_time_t* time)
{
	float hour_angle = ((time->hour % 12) * 30.0f + time->min * 0.5f) * DEG_TO_RAD;
	float min_angle = time->min * 6.0f * DEG_TO_RAD;
	float sec_angle = time->sec * 6.0f * DEG_TO_RAD;

	scr_clock_analog_draw_hand(sinf(hour_angle), cosf(hour_angle),
	                           ANALOG_HOUR_LEN, 3);
	scr_clock_analog_draw_hand(sinf(min_angle), cosf(min_angle),
	                           ANALOG_MIN_LEN, 2);
	scr_clock_analog_draw_hand(sinf(sec_angle), cosf(sec_angle),
	                           ANALOG_SEC_LEN, 1);

	view_render.fillCircle(ANALOG_CX, ANALOG_CY, ANALOG_HUB_R, WHITE);
}

void scr_clock_analog_draw_face()
{
	view_render.drawCircle(ANALOG_CX, ANALOG_CY, ANALOG_R, WHITE);

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);

	for (uint8_t h = 1; h <= 12; h++)
	{
		float angle = h * 30.0f * DEG_TO_RAD;
		float s = sinf(angle);
		float c = cosf(angle);

		uint8_t tick_len = (h % 3 == 0) ? ANALOG_TICK_MAJOR : ANALOG_TICK_MINOR;

		int16_t tx1 = ANALOG_CX + (int16_t)(s * ANALOG_R);
		int16_t ty1 = ANALOG_CY - (int16_t)(c * ANALOG_R);
		int16_t tx2 = ANALOG_CX + (int16_t)(s * (ANALOG_R - tick_len));
		int16_t ty2 = ANALOG_CY - (int16_t)(c * (ANALOG_R - tick_len));

		view_render.drawLine(tx1, ty1, tx2, ty2, WHITE);

		int16_t nx = ANALOG_CX + (int16_t)(s * ANALOG_NUM_R);
		int16_t ny = ANALOG_CY - (int16_t)(c * ANALOG_NUM_R);

		if (h < 10)
		{
			char buf[2] = {(char)('0' + h), '\0'};
			view_render.setCursor(nx - 2, ny - 3);
			view_render.print(buf);
		}
		else
		{
			char buf[3] = {'1', (char)('0' + h - 10), '\0'};
			view_render.setCursor(nx - 5, ny - 3);
			view_render.print(buf);
		}
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
	scr_clock_analog_draw_hands(&state.time);
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
