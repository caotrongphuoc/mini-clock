#include "zw_game_car.h"
#include "app_eeprom.h"

zw_game_car_t car[NUM_LANES];
static const int8_t lane_y[NUM_LANES] = LANE_Y;

static int8_t find_nearest_car(uint8_t zy)
{
    int8_t best = -1;
    int32_t best_dist = CAR_HIT_RANGE_Y + 1;
    for (uint8_t i = 0; i < NUM_LANES; i++)
    {
        if (!car[i].visible || car[i].running)
            continue;
        int32_t dist = (int32_t)zy - (int32_t)car[i].y;
        if (dist < 0)
            dist = -dist;
        if (dist < best_dist)
        {
            best_dist = dist;
            best = i;
        }
    }
    return best;
}

bool zw_game_car_check_hit(uint8_t c, uint8_t z)
{
    int32_t dy = (int32_t)zombie[z].y - (int32_t)car[c].y;
    if (dy < 0) dy = -dy;
    return (dy <= CAR_HIT_RANGE_Y) &&
           (zombie[z].x + (int32_t)ZOMBIE_MIN_LEFT_OFFSET <= (int32_t)(car[c].x + SIZE_BITMAP_CAR_X));
}

void zw_game_car_handle(ak_msg_t *msg)
{
    switch (msg->sig)
    {
    case ZW_GAME_CAR_SETUP:
    {
        APP_DBG_SIG("ZW_GAME_CAR_SETUP\n");
        for (uint8_t i = 0; i < NUM_LANES; i++)
        {
            car[i].x = AXIS_X_CAR;
            car[i].y = lane_y[i];
            car[i].lane = i;
            car[i].visible = (settingsetup.num_car >> i) & 1;
            car[i].running = false;
            car[i].action_image = 1;
        }
    }
    break;

    case ZW_GAME_CAR_RUN:
    {
        APP_DBG_SIG("ZW_GAME_CAR_RUN\n");
        for (uint8_t i = 0; i < NUM_ZOMBIES; i++)
        {
            if (zombie[i].visible != WHITE)
                continue;
            if (zombie[i].x <= -(int32_t)ZOMBIE_MIN_LEFT_OFFSET)
            {
                int8_t m = find_nearest_car(zombie[i].y);
                if (m >= 0)
                {
                    car[m].running = true;
                    zombie[i].visible = BLACK;
                }
                continue;
            }

            for (uint8_t m = 0; m < NUM_LANES; m++)
            {
                if (!car[m].visible || car[m].running)
                    continue;
                if (zw_game_car_check_hit(m, i))
                    car[m].running = true;
            }
        }

        for (uint8_t i = 0; i < NUM_LANES; i++)
        {
            if (!car[i].visible || !car[i].running)
                continue;
            car[i].x += CAR_SPEED;
            car[i].action_image++;
            if (car[i].action_image > 3)
                car[i].action_image = 1;
            if (car[i].x > LCD_WIDTH)
            {
                car[i].visible = false;
                car[i].running = false;
            }
        }
    }
    break;

    case ZW_GAME_CAR_HIT:
    {
        APP_DBG_SIG("ZW_GAME_CAR_HIT\n");
        for (uint8_t i = 0; i < NUM_LANES; i++)
        {
            if (!car[i].visible || !car[i].running)
                continue;
            for (uint8_t j = 0; j < NUM_ZOMBIES; j++)
            {
                if (zombie[j].visible != WHITE)
                    continue;
                if (!zw_game_car_check_hit(i, j))
                    continue;
                zw_game_bang_spawn(zombie[j].x, zombie[j].y);
                zw_game_score += 10;
                BUZZER_PlaySound(BUZZER_SOUND_BANG);
                zombie[j].visible = BLACK;
            }
        }
    }
    break;

    case ZW_GAME_CAR_RESET:
    {
        APP_DBG_SIG("ZW_GAME_CAR_RESET\n");
        for (uint8_t i = 0; i < NUM_LANES; i++)
        {
            car[i].x = AXIS_X_CAR;
            car[i].y = lane_y[i];
            car[i].visible = false;
            car[i].running = false;
        }
    }
    break;

    default:
        break;
    }
}