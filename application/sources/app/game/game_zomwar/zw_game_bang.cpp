#include "zw_game_bang.h"

zw_game_bang_t bang[NUM_BANG];

void zw_game_bang_reset_all()
{
	for (uint8_t i = 0; i < NUM_BANG; i++)
	{
		bang[i].visible = BLACK;
		bang[i].action_image = 1;
	}
}

void zw_game_bang_spawn(int16_t x, uint8_t y)
{
	for (uint8_t bk = 0; bk < NUM_BANG; bk++)
	{
		if (bang[bk].visible == WHITE)
			continue;
		bang[bk].visible = WHITE;
		bang[bk].x = (x + BANG_SPAWN_OFFSET_X > 0) ? (uint8_t)(x + BANG_SPAWN_OFFSET_X) : 0;
		bang[bk].y = (y >= BANG_SPAWN_OFFSET_Y) ? y - BANG_SPAWN_OFFSET_Y : 0;
		bang[bk].action_image = 1;
		break;
	}
}

void zw_game_bang_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case ZW_GAME_BANG_SETUP:
	{
		APP_DBG_SIG("ZW_GAME_BANG_SETUP\n");
		zw_game_bang_reset_all();
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
		zw_game_bang_reset_all();
	}
	break;

	default:
		break;
	}
}