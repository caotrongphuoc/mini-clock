#include "scr_zomwar_game.h"

/*****************************************************************************/
/* Variable Declaration - Zomwar game screen */
/*****************************************************************************/
static uint8_t zw_game_state;



/*****************************************************************************/
/* View - Zomwar game screen*/
/*****************************************************************************/
void zw_game_frame_display() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(8,55);
	view_render.print("Wave:");
	//view_render.print(wave_level);
	view_render.setCursor(64,55);
	view_render.print("Score:");
	//view_render.print(zw_game_score);
	view_render.drawLine(0, LCD_HEIGHT, 	LCD_WIDTH, LCD_HEIGHT,		WHITE);
	view_render.drawLine(0, LCD_HEIGHT-11, 	LCD_WIDTH, LCD_HEIGHT-11,	WHITE);
	view_render.drawRect(0, 0, 128, 64, 1);
}

void zw_game_gunner_display() {
	if (gunner.visible == WHITE) {
		const unsigned char* frame = (gunner.action_image == 2) ? bitmap_gunner_II : bitmap_gunner_I;
		view_render.drawBitmap( gunner.x, 
								gunner.y - 10, 
								frame, 
								SIZE_BITMAP_GUNNER_X, 
								SIZE_BITMAP_GUNNER_Y, 
								WHITE);
	}
}

static void view_scr_zomwar_game();

view_dynamic_t dyn_view_item_zomwar_game = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_zomwar_game
};

view_screen_t scr_zomwar_game = {
	&dyn_view_item_zomwar_game,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_zomwar_game() {
	if (zw_game_state == GAME_PLAY) {
        zw_game_frame_display();
		zw_game_gunner_display();
	}
    else if (zw_game_state == GAME_OVER) {
		view_render.clear();
	}
}

/*****************************************************************************/
/* Handle - Zomwar game screen */
/*****************************************************************************/
void scr_zw_game_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("ZW_GAME SCREEN_ENTRY\n");
		zw_game_state = GAME_PLAY;
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_SETUP);
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE, ZW_GAME_GUNNER_UPDATE_INTERVAL, TIMER_PERIODIC);
	}
		break;

	case AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE: {
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_UPDATE);
	}
		break;

	case AC_DISPLAY_BUTON_UP_PRESSED: {
		APP_DBG_SIG("ZW_GAME BTN_UP_PRESSED\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_UP);
	}
		break;

	case AC_DISPLAY_BUTON_UP_RELEASED: {
		APP_DBG_SIG("ZW_GAME BTN_UP_RELEASED\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_STOP);
	}
		break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED: {
		APP_DBG_SIG("ZW_GAME BTN_DOWN_PRESSED\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_DOWN);
	}
		break;

	case AC_DISPLAY_BUTON_DOWN_RELEASED: {
		APP_DBG_SIG("ZW_GAME BTN_DOWN_RELEASED\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_STOP);
	}
		break;

	case AC_DISPLAY_BUTON_MODE_RELEASED: {
		APP_DBG_SIG("ZW_GAME BTN_MODE_RELEASED\n");
		zw_game_state = GAME_OFF;
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE_BALL_MOVING_UPDATE);
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_RESET);
		SCREEN_TRAN(scr_idle_handle, &scr_idle);
	}
		break;

	default:
		break;
	}
}