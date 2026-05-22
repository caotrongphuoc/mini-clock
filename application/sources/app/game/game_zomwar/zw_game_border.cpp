#include "zw_game_border.h"

zw_game_border_t border;
uint32_t zw_game_score = 0;
uint8_t zw_zombie_speed = 3;

void zw_game_border_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_BORDER_SETUP: {
        APP_DBG_SIG("ZW_GAME_BORDER_SETUP\n");
        border.x = AXIS_X_BORDER; 
        border.visible = WHITE; 
        border.action_image = 0; 
    }
        break;

    case ZW_GAME_CHECK_GAME_OVER: {
        APP_DBG_SIG("ZW_GAME_CHECK_GAME_OVER\n");
        for (uint8_t i = 0; i < NUM_ZOMBIES; i++) { 
            if (zombie[i].visible != WHITE) continue; 
            if (zombie[i].x <= -(int32_t)ZOMBIE_MIN_LEFT_OFFSET) { 
                uint8_t lane = (uint8_t)((zombie[i].y - ZOMBIE_Y_MIN) / 10); 
                if (lane >= NUM_LANES) lane = NUM_LANES - 1; 
                if (!car[lane].visible) { 
                    task_post_pure_msg(ZW_GAME_SCREEN_ID, ZW_GAME_RESET); 
                } 
            } 
        } 
    }
        break;

    case ZW_GAME_ZOMBIE_KILLED: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_KILLED\n");
        zw_game_score += 10;
        //BUZZER_PlayTones(tones_bang);
        BUZZER_PlaySound(BUZZER_SOUND_BANG);
    }
        break;

    case ZW_GAME_LEVEL_UP: {
        APP_DBG_SIG("ZW_GAME_LEVEL_UP\n");
        if (zw_zombie_speed < ZOMBIE_SPEED_MAX) { 
            zw_zombie_speed++; 
        } 
    }
        break;

    case ZW_GAME_BORDER_RESET: {
        APP_DBG_SIG("ZW_GAME_BORDER_RESET\n");
        border.x = AXIS_X_BORDER; 
        border.visible = BLACK; 
        zw_game_score = 0; 
    }
        break;

    default:
        break;
    }
}