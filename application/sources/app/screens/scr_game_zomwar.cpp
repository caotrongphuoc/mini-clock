#include "scr_game_zomwar.h"

static uint8_t zw_game_state;

void zw_game_frame_display() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(8,55);
	view_render.print("Wave:");
	//view_render.print(wave_level);
	view_render.setCursor(64,55);
	view_render.print("Score:");
	view_render.print(zw_game_score);
	view_render.drawLine(0, LCD_HEIGHT-11, 	LCD_WIDTH, LCD_HEIGHT-11,	WHITE);
	view_render.drawRect(0, 0, 128, 64, 1);
}

static void view_scr_game_zomwar();

view_dynamic_t dyn_view_zomwar = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_zomwar
};

view_screen_t scr_game_zomwar = {
	&dyn_view_zomwar,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

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

void zw_game_bullet_display() {
	for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) {
		if (bullet[i].visible == WHITE) {
			view_render.drawBitmap(	bullet[i].x, 
									bullet[i].y, 
									bitmap_bullet, 
									SIZE_BITMAP_BULLET_X, 
									SIZE_BITMAP_BULLET_Y, 
									WHITE);
		}
	}
}

void zw_game_zombie_display() {
	for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
		if (zombie[i].visible == WHITE) {
			if (zombie[i].action_image == 1) {
				view_render.drawBitmap(	zombie[i].x, \
										zombie[i].y, \
										bitmap_zombie_I, \
										SIZE_BITMAP_ZOMBIES_X, \
										SIZE_BITMAP_ZOMBIES_Y, \
										WHITE);
			}
			else if (zombie[i].action_image == 2) {
				view_render.drawBitmap(	zombie[i].x, \
										zombie[i].y, \
										bitmap_zombie_II, \
										SIZE_BITMAP_ZOMBIES_X, \
										SIZE_BITMAP_ZOMBIES_Y, \
										WHITE);
			}
			else if (zombie[i].action_image == 3) {
				view_render.drawBitmap(	zombie[i].x, \
										zombie[i].y, \
										bitmap_zombie_III, \
										SIZE_BITMAP_ZOMBIES_X, \
										SIZE_BITMAP_ZOMBIES_Y, \
										WHITE);
			}
		}
	}
}

void zw_game_car_display() {
	for (uint8_t i = 0; i < NUM_LANES; i++) {
		if (car[i].visible) {
			if (car[i].action_image == 1) {
				view_render.drawBitmap( car[i].x, \
										car[i].y, \
										bitmap_car_I, \
										SIZE_BITMAP_CAR_X, \
										SIZE_BITMAP_CAR_Y, \
										WHITE);
			}
			else if (car[i].action_image == 2) {
				view_render.drawBitmap( car[i].x, \
										car[i].y, \
										bitmap_car_II, \
										SIZE_BITMAP_CAR_X, \
										SIZE_BITMAP_CAR_Y, \
										WHITE);
			}
			else if (car[i].action_image == 3) {
				view_render.drawBitmap( car[i].x, \
										car[i].y, \
										bitmap_car_III, \
										SIZE_BITMAP_CAR_X, \
										SIZE_BITMAP_CAR_Y, \
										WHITE);
			}
		}
	}
}


void zw_game_tombstone_display() {
	static const uint8_t ly[NUM_LANES] = LANE_Y;
	for (uint8_t i = 0; i < NUM_TOMBSTONES; i++) {
		if (!tombstones[i].active) continue;
		view_render.drawBitmap(
			tombstones[i].x,
			ly[tombstones[i].lane],
			bitmap_tombstone,
			SIZE_BITMAP_TOMBSTONE_X,
			SIZE_BITMAP_TOMBSTONE_Y,
			WHITE);
	}
}

void zw_game_bang_display() {
	for (uint8_t i = 0; i < NUM_BANG; i++) {
		if (bang[i].visible == WHITE) {
			const unsigned char* frame = (bang[i].action_image == 2) ? bitmap_bang_II : bitmap_bang_I;
			view_render.drawBitmap( bang[i].x, 
									bang[i].y, 
									frame, 
									SIZE_BITMAP_BANG_I_X, 
									SIZE_BITMAP_BANG_I_Y, 
									WHITE);
		}
	}
}

void zw_game_grass_display() {
	static const uint8_t ly[NUM_LANES] = LANE_Y;
	for (uint8_t l = 0; l < NUM_LANES; l++) {
		uint8_t gy = ly[l] + 9;
		for (uint8_t x = 0; x < 128; x += 5) {
			view_render.drawPixel(x,     gy,     WHITE);
			view_render.drawPixel(x + 1, gy,     WHITE);
			view_render.drawPixel(x + 2, gy,     WHITE);
		}
	}
}

void view_scr_game_zomwar() {
	if (zw_game_state == GAME_PLAY) {
        zw_game_frame_display();
		zw_game_gunner_display();
		zw_game_bullet_display();
		zw_game_zombie_display();
		zw_game_car_display();
		zw_game_tombstone_display();
		zw_game_bang_display();
		zw_game_grass_display();
	}
    else if (zw_game_state == GAME_OVER) {
		view_render.clear();
	}
}

void scr_game_zomwar_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("ZW_GAME SCREEN_ENTRY\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID,   	ZW_GAME_GUNNER_SETUP);
		task_post_pure_msg(ZW_GAME_BULLET_ID,   	ZW_GAME_BULLET_SETUP);
		task_post_pure_msg(ZW_GAME_ZOMBIE_ID, 		ZW_GAME_ZOMBIE_SETUP);
		task_post_pure_msg(ZW_GAME_CAR_ID, 	    	ZW_GAME_CAR_SETUP);
		task_post_pure_msg(ZW_GAME_TOMBSTONE_ID,	ZW_GAME_TOMBSTONE_SETUP);
		task_post_pure_msg(ZW_GAME_BANG_ID, 		ZW_GAME_BANG_SETUP);
		task_post_pure_msg(ZW_GAME_BORDER_ID,   	ZW_GAME_BORDER_SETUP);
		
		zw_game_state = GAME_PLAY;
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);

        timer_set(AC_TASK_DISPLAY_ID, 
			ZW_GAME_TIME_TICK, 
			ZW_GAME_TIME_TICK_INTERVAL, 
			TIMER_PERIODIC);
	}
		break;

	case ZW_GAME_TIME_TICK: {
		APP_DBG_SIG("ZW_GAME_TIME_TICK\n");
			task_post_pure_msg(ZW_GAME_GUNNER_ID, 		ZW_GAME_GUNNER_UPDATE);
			task_post_pure_msg(ZW_GAME_BULLET_ID, 		ZW_GAME_BULLET_RUN);
			task_post_pure_msg(ZW_GAME_ZOMBIE_ID, 		ZW_GAME_ZOMBIE_RUN);
			task_post_pure_msg(ZW_GAME_ZOMBIE_ID, 		ZW_GAME_ZOMBIE_DETONATOR);
			task_post_pure_msg(ZW_GAME_TOMBSTONE_ID,	ZW_GAME_TOMBSTONE_UPDATE);
			task_post_pure_msg(ZW_GAME_TOMBSTONE_ID,	ZW_GAME_TOMBSTONE_SPAWN);
			task_post_pure_msg(ZW_GAME_CAR_ID, 			ZW_GAME_CAR_RUN);
			task_post_pure_msg(ZW_GAME_BANG_ID, 		ZW_GAME_BANG_UPDATE);
			task_post_pure_msg(ZW_GAME_BORDER_ID, 		ZW_GAME_CHECK_GAME_OVER);
			task_post_pure_msg(ZW_GAME_BORDER_ID, 		ZW_GAME_LEVEL_UP);
	}
		break;

    case ZW_GAME_RESET: {
        APP_DBG_SIG("ZW_GAME_RESET\n");
        timer_remove_attr(AC_TASK_DISPLAY_ID, 		ZW_GAME_TIME_TICK);
        task_post_pure_msg(ZW_GAME_GUNNER_ID, 		ZW_GAME_GUNNER_RESET);
		task_post_pure_msg(ZW_GAME_BULLET_ID, 		ZW_GAME_BULLET_RESET);
		task_post_pure_msg(ZW_GAME_ZOMBIE_ID, 		ZW_GAME_ZOMBIE_RESET);
		task_post_pure_msg(ZW_GAME_CAR_ID, 			ZW_GAME_CAR_RESET);
		task_post_pure_msg(ZW_GAME_TOMBSTONE_ID,	ZW_GAME_TOMBSTONE_RESET);
		task_post_pure_msg(ZW_GAME_BANG_ID, 		ZW_GAME_BANG_RESET);
		task_post_pure_msg(ZW_GAME_BORDER_ID, 		ZW_GAME_BORDER_RESET);



        timer_set(AC_TASK_DISPLAY_ID, 
			ZW_GAME_TIME_TICK, 
			ZW_GAME_TIME_TICK_INTERVAL, 
			TIMER_PERIODIC);
	}
        break;        

	case AC_DISPLAY_BUTTON_UP_PRESSED: {
		APP_DBG_SIG("ZW_GAME BTN_UP_PRESSED\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_UP);
	}
		break;

	// case AC_DISPLAY_BUTTON_UP_RELEASED: {
	// 	APP_DBG_SIG("ZW_GAME BTN_UP_RELEASED\n");
	// 	task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_STOP);
	// }
	// 	break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
		APP_DBG_SIG("ZW_GAME BTN_DOWN_PRESSED\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_DOWN);
	}
		break;

	// case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
	// 	APP_DBG_SIG("ZW_GAME BTN_DOWN_RELEASED\n");
	// 	task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_STOP);
	// }
	// 	break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED: {
		APP_DBG_SIG("ZW_GAME BTN_MODE_PRESSED\n");
		if(zw_game_state == GAME_PLAY) {
		task_post_pure_msg(ZW_GAME_BULLET_ID, ZW_GAME_BULLET_SHOOT);
		}
	}
		break;
	
	case ZW_GAME_EXIT_GAME: {
		APP_DBG_SIG("ZW_GAME_EXIT_GAME\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, ZW_GAME_TIME_TICK);
		zw_game_state = GAME_OFF;
		SCREEN_TRAN(scr_idle_handle, &scr_idle);
	}
		break;

	default:
		break;
	}
}