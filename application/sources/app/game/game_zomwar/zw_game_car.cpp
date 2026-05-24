#include "zw_game_car.h"
#include "app_eeprom.h"

zw_game_car_t car[NUM_LANES];
static const int8_t lane_y[NUM_LANES] = LANE_Y;
static bool game_active = false;

static int8_t find_nearest_mower(uint32_t zy) {
    int8_t best = -1;
    int32_t best_dist = 999;
    for (uint8_t i = 0; i < NUM_LANES; i++) {
        if (!car[i].visible || car[i].running) continue;
        int32_t dist = (int32_t)zy - (int32_t)car[i].y;
        if (dist < 0) dist = -dist;
        if (dist < best_dist) {
            best_dist = dist;
            best = i;
        }
    }
    if (best >= 0 && best_dist > CAR_HIT_RANGE_Y) {
        best = -1;
    }
    return best;
}

void zw_game_car_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_CAR_SETUP: 
        APP_DBG_SIG("ZW_GAME_CAR_SETUP\n");
        game_active = true; 
        for (uint8_t i = 0; i < NUM_LANES; i++) { 
            car[i].x       = AXIS_X_CAR; 
            car[i].y       = lane_y[i]; 
            car[i].lane    = i; 
            car[i].visible = (settingdata.num_car >> i) & 1;
            car[i].running = false;
            car[i].action_image = 1; 
        } 
        break;

    case ZW_GAME_CAR_RUN: {
        APP_DBG_SIG("ZW_GAME_CAR_RUN\n");
        if (!game_active) break;

        // Vong 1: zombie cham mep trai -> kich hoat xe cuu (neu con); hoac zombie dam vao xe dang dung
        for (uint8_t i = 0; i < zombie_count; ) {
            if (zombie[i].x <= -(int32_t)ZOMBIE_MIN_LEFT_OFFSET) {
                int8_t m = find_nearest_mower(zombie[i].y);
                if (m >= 0) {
                    car[m].running = true;
                    // co xe cuu -> xoa zombie bang swap-remove
                    zombie_count--;
                    zombie[i] = zombie[zombie_count];
                    zombie[zombie_count].visible = BLACK;
                    continue; // xet lai phan tu vua hoan doi vao slot i
                }
                // khong con xe cuu -> de zombie o lai mep trai, border lo game over
                i++;
                continue;
            }

            for (uint8_t m = 0; m < NUM_LANES; m++) {
                if (!car[m].visible || car[m].running) continue;
                int32_t dy = (int32_t)zombie[i].y - (int32_t)car[m].y;
                if (dy < 0) dy = -dy;
                if (dy <= CAR_HIT_RANGE_Y) {
                    if (zombie[i].x + (int32_t)ZOMBIE_MIN_LEFT_OFFSET <= (int32_t)(car[m].x + SIZE_BITMAP_CAR_X)) {
                        car[m].running = true;
                    }
                }
            }
            i++;
        }

        // Vong 2: xe dang chay -> can zombie cung lane
        for (uint8_t i = 0; i < NUM_LANES; i++) {
            if (!car[i].visible || !car[i].running) continue;
            car[i].x += CAR_SPEED;
            car[i].action_image++;
            if (car[i].action_image > 3) car[i].action_image = 1;

            for (uint8_t j = 0; j < zombie_count; ) {
                int32_t dy = (int32_t)zombie[j].y - (int32_t)car[i].y;
                if (dy < 0) dy = -dy;
                if (dy > CAR_HIT_RANGE_Y) { j++; continue; }
                uint8_t fidx_c = (zombie[j].action_image == 2) ? 1 : 0;
                bool overlap_x = false;
                for (uint8_t r = 0; r < SIZE_BITMAP_ZOMBIES_Y && !overlap_x; r++) {
                    int32_t lx = zombie[j].x + (int32_t)ZOMBIE_LEFT_PX[fidx_c][r];
                    if (lx >= (int32_t)(car[i].x + 4) && lx < (int32_t)(car[i].x + SIZE_BITMAP_CAR_X)) {
                        overlap_x = true;
                    }
                }
                if (overlap_x) {
                    uint8_t bk = bang_alloc_slot();
                    bang[bk].visible      = WHITE;
                    bang[bk].x            = car[i].x;
                    bang[bk].y            = car[i].y;
                    bang[bk].action_image = 1;
                    zw_game_score += 10;
                    BUZZER_PlaySound(BUZZER_SOUND_BANG);
                    zombie_count--;
                    zombie[j] = zombie[zombie_count];
                    zombie[zombie_count].visible = BLACK;
                } else {
                    j++;
                }
            }

            if (car[i].x > LCD_WIDTH) {
                car[i].visible = false;
                car[i].running = false;
            }
        }
    }
        break;

    case ZW_GAME_CAR_RESET: {
        APP_DBG_SIG("ZW_GAME_CAR_RESET\n");
        game_active = false; 
        for (uint8_t i = 0; i < NUM_LANES; i++) { 
            car[i].x       = AXIS_X_CAR; 
            car[i].y       = lane_y[i]; 
            car[i].visible = false; 
            car[i].running = false; 
        } 
    }
        break;

    default:
        break;
    }
}