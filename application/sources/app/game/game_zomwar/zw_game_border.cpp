#include "zw_game_border.h"
#include "zw_game_car.h"

zw_game_border_t border;
uint16_t zw_game_score = 0;
uint16_t zw_game_last_wave_score = 0;
uint8_t zw_game_wave_level = 0;

void zw_game_border_handle(ak_msg_t* msg) {
    switch (msg->sig) {
        case ZW_GAME_BORDER_SETUP:
        APP_DBG_SIG("ZW_GAME_BORDER_SETUP\n");
        border.x = AXIS_X_BORDER;
        border.visible = WHITE;
        break;
        
        case ZW_GAME_LEVEL_UP:
        APP_DBG_SIG("ZW_GAME_LEVEL_UP\n");
        zw_game_wave_level++;
        break;

        case ZW_GAME_BORDER_CHECK_GAME_OVER:
        APP_DBG_SIG("ZW_GAME_BORDER_CHECK_GAME_OVER\n");
        break;

        case ZW_GAME_BORDER_RESET:
        APP_DBG_SIG("ZW_GAME_BORDER_RESET\n");
        border.x = AXIS_X_BORDER;
        border.visible = BLACK;
        zw_game_score = 0;
        zw_game_last_wave_score = 0;
        zw_game_wave_level = 0;
        break;

        default:
            break;
    }
}