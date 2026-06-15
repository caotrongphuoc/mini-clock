#include "zw_game_zombie.h"
#include "app_eeprom.h"

zw_game_zombie_t zombie[NUM_ZOMBIES];
uint8_t zw_game_zombie_speed;

void zw_game_zombie_spawn(uint8_t i)
{
	zombie[i].x = (rand() % 39) + 130;
	zombie[i].y = (rand() % (ZOMBIE_Y_MAX - ZOMBIE_Y_MIN + 1)) + ZOMBIE_Y_MIN;
	zombie[i].visible = WHITE;
	zombie[i].action_image = rand() % 3 + 1;
	zombie[i].dy = 0;
	zombie[i].zigzag_timer = rand() % 10 + 5;
	zombie[i].rising = false;
	zombie[i].rise_ticks = 0;
}

void zw_game_zombie_spawn_rise(uint8_t i, int16_t x, uint8_t y)
{
	zombie[i].x = x;
	zombie[i].y = y;
	zombie[i].visible = WHITE;
	zombie[i].action_image = 1;
	zombie[i].dy = 0;
	zombie[i].zigzag_timer = 0;
	zombie[i].rising = true;
	zombie[i].rise_ticks = ZOMBIE_RISE_TICKS;
}

bool zw_game_zombie_check_hit(uint8_t b, uint8_t z)
{
	return ((int16_t)bullet[b].x + SIZE_BITMAP_BULLET_X > zombie[z].x + ZOMBIE_HITBOX_LEFT_OFFSET) &&
	       ((int16_t)bullet[b].x < zombie[z].x + ZOMBIE_HITBOX_RIGHT_OFFSET) &&
	       ((int16_t)bullet[b].y + SIZE_BITMAP_BULLET_Y > zombie[z].y + ZOMBIE_HITBOX_TOP_OFFSET) &&
	       ((int16_t)bullet[b].y < zombie[z].y + ZOMBIE_HITBOX_BOTTOM_OFFSET);
}

void zw_game_zombie_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case ZW_GAME_ZOMBIE_SETUP:
	{
		APP_DBG_SIG("ZW_GAME_ZOMBIE_SETUP\n");
		zw_game_zombie_speed = settingsetup.zombie_speed;
		for (uint8_t i = 0; i < NUM_ZOMBIES; i++)
		{
			zombie[i].visible = BLACK;
		}
		for (uint8_t i = 0; i < NUM_ZOMBIES_INIT; i++)
		{
			zw_game_zombie_spawn(i);
		}
	}
	break;

	case ZW_GAME_ZOMBIE_RUN:
	{
		APP_DBG_SIG("ZW_GAME_ZOMBIE_RUN\n");
		uint8_t alive = 0;
		for (uint8_t i = 0; i < NUM_ZOMBIES; i++)
		{
			if (zombie[i].visible != WHITE)
				continue;
			alive++;
			if (zombie[i].rising)
			{
				if (zombie[i].rise_ticks > 0)
				{
					zombie[i].y--;
					zombie[i].rise_ticks--;
				}
				else
				{
					zombie[i].rising = false;
					zombie[i].zigzag_timer = rand() % 10 + 5;
				}
				zombie[i].action_image++;
				if (zombie[i].action_image > 3)
					zombie[i].action_image = 1;
				continue;
			}
			if (zombie[i].x - zw_game_zombie_speed < -ZOMBIE_MIN_LEFT_OFFSET)
			{
				zombie[i].x = -ZOMBIE_MIN_LEFT_OFFSET;
			}
			else
			{
				zombie[i].x -= zw_game_zombie_speed;
			}
			if (zombie[i].zigzag_timer > 0)
			{
				zombie[i].zigzag_timer--;
			}
			else
			{
				zombie[i].dy = (int8_t)(rand() % 3) - 1;
				zombie[i].zigzag_timer = rand() % 10 + 5;
			}
			int16_t new_y = (int16_t)zombie[i].y + zombie[i].dy;
			if (new_y < ZOMBIE_Y_MIN)
			{
				new_y = ZOMBIE_Y_MIN;
				zombie[i].dy = 0;
			}
			if (new_y > ZOMBIE_Y_MAX)
			{
				new_y = ZOMBIE_Y_MAX;
				zombie[i].dy = 0;
			}
			zombie[i].y = (uint8_t)new_y;
			zombie[i].action_image++;
			if (zombie[i].action_image > 3)
				zombie[i].action_image = 1;
		}
		for (uint8_t i = 0; i < NUM_ZOMBIES && alive < NUM_ZOMBIES_INIT; i++)
		{
			if (zombie[i].visible == WHITE)
				continue; // slot dang dung
			zw_game_zombie_spawn(i);
			alive++;
		}
	}
	break;

	case ZW_GAME_ZOMBIE_DETONATOR:
	{
		APP_DBG_SIG("ZW_GAME_ZOMBIE_DETONATOR\n");
		for (uint8_t i = 0; i < NUM_ZOMBIES; i++)
		{
			if (zombie[i].visible != WHITE)
				continue;
			if (zombie[i].rising)
				continue;
			for (uint8_t j = 0; j < NUM_BULLET; j++)
			{
				if (bullet[j].visible != WHITE)
					continue;
				if (!zw_game_zombie_check_hit(j, i))
					continue;
				bullet[j].visible = BLACK;
				bullet[j].x = 0;
				zw_game_bang_spawn(zombie[i].x, zombie[i].y);
				zw_game_score += 10;
				BUZZER_PlaySound(BUZZER_SOUND_BANG);
				zombie[i].visible = BLACK;
				break;
			}
		}
	}
	break;

	case ZW_GAME_ZOMBIE_WAVE_SPAWN:
	{
		APP_DBG_SIG("ZW_GAME_ZOMBIE_WAVE_SPAWN\n");
		if (zw_game_zombie_speed < ZOMBIE_SPEED_MAX)
		{
			zw_game_zombie_speed++;
		}
		uint8_t spawned = 0;
		for (uint8_t i = 0; i < NUM_ZOMBIES && spawned < ZOMBIE_WAVE_SPAWN; i++)
		{
			if (zombie[i].visible == WHITE)
				continue;
			zw_game_zombie_spawn(i);
			spawned++;
		}
	}
	break;

	case ZW_GAME_ZOMBIE_RESET:
	{
		APP_DBG_SIG("ZW_GAME_ZOMBIE_RESET\n");
		for (uint8_t i = 0; i < NUM_ZOMBIES; i++)
		{
			zombie[i].visible = BLACK;
		}
	}
	break;

	case ZW_GAME_ZOMBIE_SETUP_MENU:
	{
		APP_DBG_SIG("ZW_GAME_ZOMBIE_SETUP_MENU\n");
		for (uint8_t i = 0; i < NUM_ZOMBIES; i++)
		{
			zombie[i].visible = BLACK;
		}
		zombie[0].x = LCD_WIDTH + 3;
		zombie[0].y = AXIS_Y_GUNNER - 10;
		zombie[0].rising = false;
		zombie[0].visible = WHITE;
		zombie[0].action_image = 1;
	}
	break;

	case ZW_GAME_ZOMBIE_RUN_MENU:
	{
		APP_DBG_SIG("ZW_GAME_ZOMBIE_RUN_MENU\n");
		zombie[0].x -= MENU_ZOMBIE_SPEED;
		zombie[0].visible = WHITE;
		zombie[0].action_image++;
		if (zombie[0].action_image > 3)
			zombie[0].action_image = 1;
		if (zombie[0].x < -SIZE_BITMAP_ZOMBIES_X)
		{
			zombie[0].x = LCD_WIDTH + 3;
			zombie[0].y = AXIS_Y_GUNNER - 10;
		}
		for (uint8_t j = 0; j < NUM_BULLET; j++)
		{
			if (bullet[j].visible != WHITE)
				continue;
			if (!zw_game_zombie_check_hit(j, 0))
				continue;
			bullet[j].visible = BLACK;
			bullet[j].x = 0;
			zw_game_bang_spawn(zombie[0].x, zombie[0].y);
			BUZZER_PlaySound(BUZZER_SOUND_BANG);
			zombie[0].x = LCD_WIDTH + 3;
			zombie[0].y = AXIS_Y_GUNNER - 10;
			break;
		}
	}
	break;

	default:
		break;
	}
}
