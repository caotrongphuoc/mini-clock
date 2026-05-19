#include "zw_game_tombstone.h"

zw_game_bullet_t tombstone[NUM_TOMBSTONES];

void zw_game_tombstone_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_TOMBSTONE_SETUP: 
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_SETUP\n");
        for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) { 
            bullet[i].x = 0;
            bullet[i].y = 0; 
            bullet[i].visible = BLACK; 
            //bullet[i].action_image = 1; 
        } 
        break;

    case ZW_GAME_TOMBSTONE_UPDATE: 
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_UPDATE\n");
        for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) { 
            if (bullet[i].visible == WHITE) {
                bullet[i].x += STEP_BULLET_AXIS_X;
                if (bullet[i].x >= MAX_AXIS_X_BULLET) { 
                    bullet[i].visible = BLACK; 
                    bullet[i].x = 0; 
                } 
            } 
        } 
        break;

    case ZW_GAME_TOMBSTONE_RESET: 
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_RESET\n");
        for (uint8_t i = 0; i < NUM_BULLET; i++) { 
            bullet[i].x = 0; 
            bullet[i].y = 0; 
            bullet[i].visible = BLACK; 
            //bullet[i].action_image = 1; 
        } 
        break;

    default:
        break;
    }
}