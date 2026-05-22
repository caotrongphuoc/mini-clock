#include "zw_game_border.h"

zw_game_border_t border;
uint32_t zw_game_score = 0;

/* Trang thai wave / warning */
uint32_t wave_last_score     = 0;
uint8_t  wave_warning_timer  = 0;
bool     wave_warning_active = false;
uint8_t  wave_level          = 0;

void zw_game_border_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_BORDER_SETUP: {
        APP_DBG_SIG("ZW_GAME_BORDER_SETUP\n");
        border.x = AXIS_X_BORDER;
        border.visible = WHITE;
        border.action_image = 0;
        /* reset tien trinh wave cho van choi moi */
        wave_last_score     = 0;
        wave_warning_timer  = 0;
        wave_warning_active = false;
        wave_level          = 0;
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

    case ZW_GAME_LEVEL_UP: {
        APP_DBG_SIG("ZW_GAME_LEVEL_UP\n");
        /* Bat warning khi diem vuot nguong wave ke tiep */
        if (!wave_warning_active && zw_game_score >= wave_last_score + WAVE_SCORE_INTERVAL) {
            wave_warning_active = true;
            wave_warning_timer  = WARNING_BLINK_DURATION;
        }
        /* Dang warning: dem nguoc; het gio thi sang wave moi */
        if (wave_warning_active) {
            if (wave_warning_timer > 0) {
                wave_warning_timer--;
            } else {
                wave_warning_active = false;
                wave_last_score    += WAVE_SCORE_INTERVAL;
                wave_level++;
                /* Level up: zombie nhanh hon (tang bien runtime, khong dung settingdata) */
                if (zw_game_zombie_speed < ZOMBIE_SPEED_MAX) {
                    zw_game_zombie_speed++;
                }
                /* Sinh 1 dot WAVE_SPAWN_COUNT zombie tu mep phai */
                uint8_t spawned = 0;
                for (uint8_t i = 0; i < NUM_ZOMBIES && spawned < WAVE_SPAWN_COUNT; i++) {
                    if (zombie[i].visible != WHITE) {
                        zombie[i].x            = (rand() % 39) + 130;
                        zombie[i].y            = (rand() % (ZOMBIE_Y_MAX - ZOMBIE_Y_MIN + 1)) + ZOMBIE_Y_MIN;
                        zombie[i].visible      = WHITE;
                        zombie[i].action_image = rand() % 3 + 1;
                        zombie[i].dy           = 0;
                        zombie[i].zigzag_timer = rand() % 10 + 5;
                        zombie[i].rising       = false;
                        zombie[i].rise_ticks   = 0;
                        spawned++;
                    }
                }
            }
        }
    }
        break;

    case ZW_GAME_BORDER_RESET: {
        APP_DBG_SIG("ZW_GAME_BORDER_RESET\n");
        border.x = AXIS_X_BORDER;
        border.visible = BLACK;
        zw_game_score = 0;
        wave_last_score     = 0;
        wave_warning_timer  = 0;
        wave_warning_active = false;
        wave_level          = 0;
    }
        break;

    default:
        break;
    }
}