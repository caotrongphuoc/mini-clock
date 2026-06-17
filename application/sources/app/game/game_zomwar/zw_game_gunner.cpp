#include "zw_game_gunner.h"

zw_game_gunner_t gunner;
static uint8_t gunner_y;

void zw_game_gunner_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{

	case ZW_GAME_GUNNER_SETUP:
	{
		APP_DBG_SIG("ZW_GAME_GUNNER_SETUP\n");
		gunner.x = GUNNER_AXIS_X;
		gunner.y = GUNNER_AXIS_Y;
		gunner.visible = WHITE;
		gunner.action_image = 1;
		gunner_y = GUNNER_AXIS_Y;
	}
	break;

	case ZW_GAME_GUNNER_UP:
	{
		APP_DBG_SIG("ZW_GAME_GUNNER_UP\n");
		if (gunner_y <= GUNNER_AXIS_Y_MIN + GUNNER_STEP_AXIS_Y)
			gunner_y = GUNNER_AXIS_Y_MIN;
		else
			gunner_y -= GUNNER_STEP_AXIS_Y;
	}
	break;

	case ZW_GAME_GUNNER_DOWN:
	{
		APP_DBG_SIG("ZW_GAME_GUNNER_DOWN\n");
		gunner_y += GUNNER_STEP_AXIS_Y;
		if (gunner_y > GUNNER_AXIS_Y_MAX)
		{
			gunner_y = GUNNER_AXIS_Y_MAX;
		}
	}
	break;

	case ZW_GAME_GUNNER_UPDATE:
	{
		gunner.y = gunner_y;
		if (gunner.action_image == 2)
		{
			gunner.action_image = 1;
		}
	}
	break;

	case ZW_GAME_GUNNER_RESET:
	{
		APP_DBG_SIG("ZW_GAME_GUNNER_RESET\n");
		gunner.x = GUNNER_AXIS_X;
		gunner.y = GUNNER_AXIS_Y;
		gunner.visible = BLACK;
		gunner_y = GUNNER_AXIS_Y;
	}
	break;

	default:
		break;
	}
}
