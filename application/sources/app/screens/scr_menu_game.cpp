#include "scr_menu_game.h"

static uint8_t shoot_tick_counter; // Dem tick de vien dan duoc ban ra theo vong lap

/*****************************************************************************/
/* View - Menu game */
/*****************************************************************************/
static void view_scr_menu_game();

view_dynamic_t dyn_view_menu_game = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_menu_game
};

view_screen_t scr_menu_game = {
	&dyn_view_menu_game,
	ITEM_NULL,
	ITEM_NULL,
	.focus_item = 0,
};

void view_scr_menu_game() {
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.setCursor(30,0); 
    view_render.print("ZOMWAR");
    view_render.drawLine(0, LCD_HEIGHT-13, LCD_WIDTH, LCD_HEIGHT-13, WHITE);
    view_render.drawBitmap(60, LCD_HEIGHT-22, bitmap_tombstone, 8 ,10, WHITE);
    zw_game_bullet_display();
    zw_game_gunner_display();
}

void scr_menu_game_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: 
		APP_DBG_SIG("SCREEN_ENTRY\n");
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_SETUP);
		task_post_pure_msg(ZW_GAME_BULLET_ID, ZW_GAME_BULLET_SETUP);
        zw_game_time_tick_setup();
		break;

        case ZW_GAME_TIME_TICK: 
		APP_DBG_SIG("ZW_GAME_TIME_TICK\n");
		task_post_pure_msg(ZW_GAME_BULLET_ID, ZW_GAME_BULLET_RUN);
        shoot_tick_counter++;
        if(shoot_tick_counter >= 20) {
            shoot_tick_counter = 0;
            task_post_pure_msg(ZW_GAME_BULLET_ID, ZW_GAME_BULLET_SHOOT);
        }
		break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: 
		SCREEN_TRAN(scr_zw_game_handle, &scr_zomwar_game);
		break;

	default:
		break;
	}
}
