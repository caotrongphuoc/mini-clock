#include "zw_game_tombstone.h"
#include "app_eeprom.h"

zw_game_tombstone_t tombstone[NUM_TOMBSTONE];
static uint8_t tombstone_spawn_timer = 0;

void zw_game_tombstone_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case ZW_GAME_TOMBSTONE_SETUP:
	{
		APP_DBG_SIG("ZW_GAME_TOMBSTONE_SETUP\n");
		tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL;

		for (uint8_t l = 0; l < NUM_LANE; l++)
		{
			tombstone[l].x = (uint8_t)(rand() % 20 + 65);
			tombstone[l].lane = l;
			tombstone[l].active = (bool)((settingsetup.tombstone_lane_1 >> l) & 1);

			tombstone[l + NUM_LANE].x = (uint8_t)(rand() % 20 + 90);
			tombstone[l + NUM_LANE].lane = l;
			tombstone[l + NUM_LANE].active = (bool)((settingsetup.tombstone_lane_2 >> l) & 1);
		}
	}
	break;

	case ZW_GAME_TOMBSTONE_SPAWN:
	{
		APP_DBG_SIG("ZW_GAME_TOMBSTONE_SPAWN\n");
		if (tombstone_spawn_timer > 0)
		{
			tombstone_spawn_timer--;
			return;
		}
		tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL;
		uint8_t tidx = (uint8_t)(rand() % NUM_TOMBSTONE);
		if (!tombstone[tidx].active)
			return;
		for (uint8_t i = 0; i < NUM_ZOMBIE; i++)
		{
			if (zombie[i].visible == WHITE)
				continue;
			int16_t x = tombstone[tidx].x;
			uint8_t y = lane_y[tombstone[tidx].lane] + SIZE_BITMAP_TOMBSTONE_Y;
			zw_game_zombie_spawn_from_tombstone(i, x, y);
			break;
		}
	}
	break;

	case ZW_GAME_TOMBSTONE_RESET:
	{
		APP_DBG_SIG("ZW_GAME_TOMBSTONE_RESET\n");
		tombstone_spawn_timer = 0;

		for (uint8_t i = 0; i < NUM_TOMBSTONE; i++)
		{
			tombstone[i].x = 0;
			tombstone[i].lane = 0;
			tombstone[i].active = false;
		}
	}
	break;

	default:
		break;
	}
}