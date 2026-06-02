#include "zw_game_border.h"

zw_game_border_t border;
uint16_t zw_game_score = 0;
uint16_t wave_last_score = 0;
uint8_t wave_warning_timer = 0;
uint8_t wave_level = 0;
bool wave_warning_active = false;

void zw_game_border_handle(ak_msg_t *msg)
{
    switch (msg->sig)
    {
    case ZW_GAME_BORDER_SETUP:
    {
        APP_DBG_SIG("ZW_GAME_BORDER_SETUP\n");
        border.x = AXIS_X_BORDER;
        wave_last_score = 0;
        wave_warning_timer = 0;
        wave_warning_active = false;
        wave_level = 0;
    }
    break;

    case ZW_GAME_CHECK_GAME_OVER:
    {
        APP_DBG_SIG("ZW_GAME_CHECK_GAME_OVER\n");
        for (uint8_t i = 0; i < NUM_ZOMBIES; i++)
        {
            if (zombie[i].visible != WHITE)
                continue;
            if (zombie[i].x <= -(int32_t)ZOMBIE_MIN_LEFT_OFFSET)
            {
                uint8_t lane = (uint8_t)((zombie[i].y - ZOMBIE_Y_MIN) / 10);
                if (lane >= NUM_LANES)
                    lane = NUM_LANES - 1;
                if (!car[lane].visible)
                {
                    task_post_pure_msg(AC_TASK_DISPLAY_ID, ZW_GAME_RESET);
                    break;
                }
            }
        }
    }
    break;

    case ZW_GAME_WAVE_CHECK:
    {
        APP_DBG_SIG("ZW_GAME_WAVE_CHECK\n");
        if (!wave_warning_active &&
            zw_game_score >= wave_last_score + WAVE_SCORE_INTERVAL)
        {
            wave_warning_active = true;
            wave_warning_timer = WARNING_BLINK_DURATION;
        }
    }
    break;

    case ZW_GAME_LEVEL_UP:
    {
        APP_DBG_SIG("ZW_GAME_LEVEL_UP\n");
        if (!wave_warning_active) break;
        if (wave_warning_timer > 0)
        {
            wave_warning_timer--;
            break;
        }
        wave_warning_active = false;
        wave_last_score += WAVE_SCORE_INTERVAL;
        wave_level++;
        if (zw_game_zombie_speed < ZOMBIE_SPEED_MAX)
        {
            zw_game_zombie_speed++;
        }
        uint8_t spawned = 0;
        for (uint8_t i = 0; i < NUM_ZOMBIES && spawned < WAVE_SPAWN_COUNT; i++)
        {
            if (zombie[i].visible == WHITE)
                continue;
            zw_game_zombie_spawn(i);
            spawned++;
        }
    }
    break;

    case ZW_GAME_BORDER_RESET:
    {
        APP_DBG_SIG("ZW_GAME_BORDER_RESET\n");
        border.x = AXIS_X_BORDER;
        zw_game_score = 0;
        wave_last_score = 0;
        wave_warning_timer = 0;
        wave_warning_active = false;
        wave_level = 0;
    }
    break;

    default:
        break;
    }
}