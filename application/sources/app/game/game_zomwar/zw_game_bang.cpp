#include "zw_game_bang.h"

zw_game_bang_t bang[NUM_BANG];
static uint8_t bang_next = 0;
uint8_t bang_alloc_slot(void) {
    uint8_t slot = bang_next;
    bang_next = (bang_next + 1) % NUM_BANG;
    return slot;
}

void zw_game_bang_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_BANG_SETUP: {
        APP_DBG_SIG("ZW_GAME_BANG_SETUP\n");
        for(uint8_t i = 0; i < NUM_BANG; i++) {
            bang[i].visible = BLACK;
            bang[i].action_image = 1;
        }
    }
        break;
    case ZW_GAME_BANG_UPDATE: {
        APP_DBG_SIG("ZW_GAME_BANG_UPDATE\n");
        for(uint8_t i = 0; i < NUM_BANG; i++) {
            if(bang[i].visible == WHITE) {
                bang[i].action_image++;
            }
            if(bang[i].action_image == 3) {
                bang[i].action_image = 1;
                bang[i].visible = BLACK;
            }
        }
    }
        break;
    case ZW_GAME_BANG_RESET: {
        APP_DBG_SIG("ZW_GAME_BANG_RESET\n");
        for(uint8_t i = 0; i < NUM_BANG; i++) {
            bang[i].visible = BLACK;
            bang[i].action_image = 1;
        }
    }
        break;
    default:
        break;
    }
}