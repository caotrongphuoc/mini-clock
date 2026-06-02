#include "zw_game_bang.h"

zw_game_bang_t bang[NUM_BANG];

void zw_game_bang_handle(ak_msg_t *msg)
{
    switch (msg->sig)
    {
    case ZW_GAME_BANG_SETUP:
    {
        APP_DBG_SIG("ZW_GAME_BANG_SETUP\n");
        for (uint8_t i = 0; i < NUM_BANG; i++)
        {
            bang[i].visible = BLACK;
            bang[i].action_image = 1;
        }
    }
    break;

    case ZW_GAME_BANG_UPDATE:
    {
        APP_DBG_SIG("ZW_GAME_BANG_UPDATE\n");
        for (uint8_t i = 0; i < NUM_BANG; i++)
        {
            if (bang[i].visible == WHITE)
            {
                if (bang[i].action_image >= 3)
                {
                    bang[i].action_image = 1;
                    bang[i].visible = BLACK;
                }
                else
                {
                    bang[i].action_image++;
                }
            }
        }
    }
    break;

    case ZW_GAME_BANG_RESET:
    {
        APP_DBG_SIG("ZW_GAME_BANG_RESET\n");
        for (uint8_t i = 0; i < NUM_BANG; i++)
        {
            bang[i].visible = BLACK;
            bang[i].action_image = 1;
        }
    }
    break;

    default:
        break;
    }
}