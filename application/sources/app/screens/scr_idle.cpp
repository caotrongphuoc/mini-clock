#include "scr_idle.h"
#include "scr_game_zomwar.h"

#define MAX_BALL_DISPLAY	(8)

class ball {
public:
	int id, x, y, slope, axis_x, axis_y, radius;

	void init() {
		axis_x = 1;
		axis_y = 1;
		slope  = (rand() % 31) - 15;
		radius = (rand() % 7) + 6;
		x      = rand() % (LCD_WIDTH  - radius);
		y      = rand() % (LCD_HEIGHT - radius);
	}

	void moving() {
		x += (axis_x > 0) ? 2 : -2;
		y += (axis_y > 0) ? (int)(2 * atan(slope)) : -(int)(2 * atan(slope));

		if (x > (LCD_WIDTH - radius) || x < radius) {
			axis_x = -axis_x;
			if (x < radius) x = radius;
		}
		if (y > (LCD_HEIGHT - radius) || y < radius) {
			axis_y = -axis_y;
			if (y < radius) y = radius;
		}
	}
};

static ball  v_idle_ball[MAX_BALL_DISPLAY];
static uint8_t ball_count = 0;
static int   ball_total   = 0;

static void view_scr_idle();

view_dynamic_t dyn_view_idle = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_idle
};

view_screen_t scr_idle = {
	&dyn_view_idle,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_idle() {
	for (uint8_t i = 0; i < ball_count; i++) {
		view_render.drawCircle(v_idle_ball[i].x, v_idle_ball[i].y, v_idle_ball[i].radius, WHITE);
	}
}

void scr_idle_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		if (ball_count == 0) {
			v_idle_ball[0].init();
			v_idle_ball[0].id = ball_total++;
			ball_count = 1;
		}
		timer_set(AC_TASK_DISPLAY_ID,
				  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE,
				  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL,
				  TIMER_PERIODIC);
	}
		break;

	case AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE: {
		for (uint8_t i = 0; i < ball_count; i++) {
			v_idle_ball[i].moving();
		}
	}
		break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE);
		SCREEN_TRAN(scr_game_menu_handle, &scr_game_menu);
	}
		break;

	case AC_DISPLAY_BUTTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_RELEASED\n");
		if (ball_count == 0) {
			timer_set(AC_TASK_DISPLAY_ID,
					  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE,
					  AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE_INTERAL,
					  TIMER_PERIODIC);
		}
		if (ball_count < MAX_BALL_DISPLAY) {
			v_idle_ball[ball_count].init();
			v_idle_ball[ball_count].id = ball_total++;
			ball_count++;
		} else {
			BUZZER_PlayTones(tones_3beep);
		}
	}
		break;

	case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_RELEASED\n");
		if (ball_count > 0) {
			ball_count--;
			ball_total--;
		}
		if (ball_count == 0) {
			timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE);
		}
	}
		break;

	default:
		break;
	}
}
