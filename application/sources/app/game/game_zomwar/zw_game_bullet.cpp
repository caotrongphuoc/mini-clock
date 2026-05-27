#include "zw_game_bullet.h"

zw_game_bullet_t bullet[MAX_NUM_BULLET];

void zw_game_bullet_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_BULLET_SETUP: {
        APP_DBG_SIG("ZW_GAME_BULLET_SETUP\n");
        for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) {
            bullet[i].x = 0;
            bullet[i].y = 0;
            bullet[i].visible = BLACK;
        }
    }
        break;

    case ZW_GAME_BULLET_RUN: {
        APP_DBG_SIG("ZW_GAME_BULLET_RUN\n");
        for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) {
            if (bullet[i].visible != WHITE) continue;
            bullet[i].x += STEP_BULLET_AXIS_X;
            if (bullet[i].x >= MAX_AXIS_X_BULLET) {
                bullet[i].visible = BLACK; // ra khoi man hinh -> tra slot
                bullet[i].x = 0;
            }
        }
    }
        break;

    case ZW_GAME_BULLET_SHOOT: {
        APP_DBG_SIG("ZW_GAME_BULLET_SHOOT\n");
        for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) {
            if (bullet[i].visible == WHITE) continue; // slot dang dung
            bullet[i].visible = WHITE;
            bullet[i].x = gunner.x + 15;
            bullet[i].y = gunner.y - 8;
            gunner.action_image = 2;
            if(zw_game_sound_enable) {
                BUZZER_PlaySound(BUZZER_SOUND_CLICK);
            }
            break; // chi ban 1 vien vao slot trong dau tien
        }
    }
        break;

    case ZW_GAME_BULLET_RESET: {
        APP_DBG_SIG("ZW_GAME_BULLET_RESET\n");
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
