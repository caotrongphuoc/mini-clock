#include "zw_game_bullet.h"

zw_game_bullet_t bullet[MAX_NUM_BULLET];
uint8_t bullet_count = 0; 

void zw_game_bullet_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_BULLET_SETUP: {
        APP_DBG_SIG("ZW_GAME_BULLET_SETUP\n");
        bullet_count = 0;
        for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) {
            bullet[i].x = 0;
            bullet[i].y = 0;
            bullet[i].visible = BLACK;
        }
    }
        break;

    case ZW_GAME_BULLET_RUN: {
        APP_DBG_SIG("ZW_GAME_BULLET_RUN\n");
        for (uint8_t i = 0; i < bullet_count; i++) {
            bullet[i].x += STEP_BULLET_AXIS_X;
            if (bullet[i].x >= MAX_AXIS_X_BULLET) {
                bullet_count--;
                bullet[i] = bullet[bullet_count];
                bullet[bullet_count].visible = BLACK;
                bullet[bullet_count].x = 0;
            }
        }
    }
        break;

    case ZW_GAME_BULLET_SHOOT: {
        APP_DBG_SIG("ZW_GAME_BULLET_SHOOT\n");
        if (bullet_count < MAX_NUM_BULLET) {
            uint8_t i = bullet_count;
            bullet[i].visible = WHITE;
            bullet[i].x = gunner.x + 15;
            bullet[i].y = gunner.y - 8;
            bullet_count++;
            gunner.action_image = 2;
            if(zw_game_sound_enable) {
                BUZZER_PlaySound(BUZZER_SOUND_CLICK);
            }
        }
    }
        break;

    case ZW_GAME_BULLET_RESET: {
        APP_DBG_SIG("ZW_GAME_BULLET_RESET\n");
        bullet_count = 0;
        for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) {
            bullet[i].x = 0;
            bullet[i].y = 0;
            bullet[i].visible = BLACK;
        }
    }
        break;

    default:
        break;
    }
}