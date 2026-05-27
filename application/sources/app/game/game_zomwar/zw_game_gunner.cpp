#include "zw_game_gunner.h"

zw_game_gunner_t gunner;
static uint8_t gunner_y ; 

void zw_game_gunner_handle(ak_msg_t* msg) {
    switch (msg->sig) {
 
    case ZW_GAME_GUNNER_SETUP: {
        APP_DBG_SIG("ZW_GAME_GUNNER_SETUP\n");
        gunner.x = AXIS_X_GUNNER;
        gunner.y = AXIS_Y_GUNNER;
        gunner.visible = WHITE;
        gunner.action_image = 1;
        gunner_y = AXIS_Y_GUNNER;
    } break;

    case ZW_GAME_GUNNER_UP: {
        APP_DBG_SIG("ZW_GAME_GUNNER_UP\n");
        gunner_y -= STEP_GUNNER_AXIS_Y;
        if(gunner_y < AXIS_Y_GUNNER_MIN) {
            gunner_y = AXIS_Y_GUNNER_MIN;
        }
        gunner.y = gunner_y; 
    } break;

    case ZW_GAME_GUNNER_DOWN: {
        APP_DBG_SIG("ZW_GAME_GUNNER_DOWN\n");
        gunner_y += STEP_GUNNER_AXIS_Y;
        if(gunner_y > AXIS_Y_GUNNER_MAX) {
            gunner_y = AXIS_Y_GUNNER_MAX;
        }
        gunner.y = gunner_y; 
    } break;

    case ZW_GAME_GUNNER_UPDATE: {
        if (gunner.action_image == 2) {
            gunner.action_image = 1;
        }
    } break;

    case ZW_GAME_GUNNER_RESET: {
        APP_DBG_SIG("ZW_GAME_GUNNER_RESET\n");
        gunner.x = AXIS_X_GUNNER;
        gunner.y = AXIS_Y_GUNNER;
        gunner.visible = BLACK;
        gunner_y = AXIS_Y_GUNNER;
    } break;

    default:
        break;
    }
}
