#include "zw_game_zombie.h"

zw_game_zombie_t zombie[NUM_ZOMBIES];
static bool game_active = false; // Check trang thai hien tai cua game
static const int32_t ZOMBIE_SPEED_TMP = 2; // fix tam, doi 1/2/3/4... ; sau nay lay tu menu Setting
const uint8_t ZOMBIE_LEFT_PX[2][SIZE_BITMAP_ZOMBIES_Y] = {
    {9, 8, 8, 9, 6, 9, 9, 9, 9, 8},
    {9, 8, 8, 9, 7, 9, 9, 9, 9, 9},
};

static const uint8_t lane_y_arr[NUM_LANES] = LANE_Y;

void spawn_zombie_from_tombstone(uint8_t i, uint8_t tidx) {
    zombie[i].x            = tombstones[tidx].x;
    zombie[i].y            = (uint32_t)lane_y_arr[tombstones[tidx].lane] + SIZE_BITMAP_TOMBSTONE_Y;
    zombie[i].visible      = WHITE;
    zombie[i].action_image = 1;
    zombie[i].dy           = 0;
    zombie[i].zigzag_timer = 0;
    zombie[i].rising       = true;
    zombie[i].rise_ticks   = ZOMBIE_RISE_TICKS;
}

void zw_game_zombie_handle(ak_msg_t* msg) {
    uint8_t active_count = 0;
    switch (msg->sig) {
    case ZW_GAME_ZOMBIE_SETUP: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_SETUP\n");
        game_active = true;
        for(uint8_t i = 0; i < NUM_ZOMBIES; i++) {
            if(i < NUM_ZOMBIES_INIT) {
                zombie[i].x            = (rand() % 39) + 130;
                zombie[i].y            = (rand() % (ZOMBIE_Y_MAX - ZOMBIE_Y_MIN + 1)) + ZOMBIE_Y_MIN;
                zombie[i].visible      = WHITE;
                zombie[i].action_image = rand() % 3 + 1;
                zombie[i].dy           = 0;
                zombie[i].zigzag_timer = rand() % 10 + 5;
                zombie[i].rising       = false;
                zombie[i].rise_ticks   = 0;
            }
        }
    }
        break;

    case ZW_GAME_ZOMBIE_RUN: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_RUN\n");
        if(!game_active) return;
        active_count = 0;
        for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
            if (zombie[i].visible != WHITE) continue;
            active_count++;
            // Xu ly zombie dang troi len tu bia mo
            if (zombie[i].rising) {
                if (zombie[i].rise_ticks > 0) {
                    zombie[i].y--;
                    zombie[i].rise_ticks--;
                } else {
                    zombie[i].rising       = false;
                    zombie[i].zigzag_timer = rand() % 10 + 5;
                }
                zombie[i].action_image++;
                if (zombie[i].action_image > 3) zombie[i].action_image = 1;
                continue;
            }
            // Zombie di chuyen sang trai
            if (zombie[i].x - ZOMBIE_SPEED_TMP < -(int32_t)ZOMBIE_MIN_LEFT_OFFSET) {
                zombie[i].x = -(int32_t)ZOMBIE_MIN_LEFT_OFFSET;
            } else {
                zombie[i].x -= ZOMBIE_SPEED_TMP;
            }
            // Xu ly di chuyen zigzag theo truc Y
            if (zombie[i].zigzag_timer > 0) {
                zombie[i].zigzag_timer--;
            } else {
                zombie[i].dy           = (int8_t)(rand() % 3) - 1;
                zombie[i].zigzag_timer = rand() % 10 + 5;
            }

            int32_t new_y = (int32_t)zombie[i].y + zombie[i].dy;
            if (new_y < ZOMBIE_Y_MIN) { new_y = ZOMBIE_Y_MIN; zombie[i].dy = 0; }
            if (new_y > ZOMBIE_Y_MAX) { new_y = ZOMBIE_Y_MAX; zombie[i].dy = 0; }
            zombie[i].y = (uint32_t)new_y;

            zombie[i].action_image++;
            if (zombie[i].action_image > 3) zombie[i].action_image = 1;
        }
        // Tu dong spawn them zombie neu thieu
        if (active_count < NUM_ZOMBIES_INIT) {
            for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
                if (zombie[i].visible != WHITE) {
                    zombie[i].x            = (rand() % 39) + 130;
                    zombie[i].y            = (rand() % (ZOMBIE_Y_MAX - ZOMBIE_Y_MIN + 1)) + ZOMBIE_Y_MIN;
                    zombie[i].visible      = WHITE;
                    zombie[i].action_image = rand() % 3 + 1;
                    zombie[i].dy           = 0;
                    zombie[i].zigzag_timer = rand() % 10 + 5;
                    zombie[i].rising       = false;
                    zombie[i].rise_ticks   = 0;
                    active_count++;
                    if (active_count >= NUM_ZOMBIES_INIT) break;
                }
            }
        }
    }
        break;

    case ZW_GAME_ZOMBIE_DETONATOR: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_DETONATOR\n");
        for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
            if (zombie[i].visible != WHITE) continue;
            if (zombie[i].rising) continue;

            for (uint8_t j = 0; j < MAX_NUM_BULLET; j++) {
                if (bullet[j].visible != WHITE) continue;

                int32_t ax = bullet[j].x;
                int32_t ay = bullet[j].y;
                int32_t mx = zombie[i].x;
                int32_t my = (int32_t)zombie[i].y;
                // Kiem tra va cham (dan co ban dinh zombie hay chua)
                bool hit = (ax + SIZE_BITMAP_BULLET_X > mx + 12) &&     // Dan qua phan than zombie 12 px
                        (ax < mx + (int32_t)SIZE_BITMAP_ZOMBIES_X) &&   // Dan chua qua ben phai zombie
                        (ay + (int32_t)SIZE_BITMAP_BULLET_Y > my) &&    // Dan khong o tren zombie
                        (ay < my + (int32_t)SIZE_BITMAP_ZOMBIES_Y);     // Dan khong o duoi zombie
                // Neu dinh luu vi tri de tao hieu ung no
                if (hit) {
                    int32_t  dead_x = zombie[i].x;
                    uint32_t dead_y = zombie[i].y;

                    zombie[i].visible = BLACK;
                    zombie[i].x       = 200;    // Day zombie ra ngoai man hinh co toa do x = 200;
                    bullet[j].visible = BLACK;  // Tieu thu (an) vien dan
                    bullet[j].x       = 0;  

                    uint8_t bk       = bang_alloc_slot();
                    bang[bk].visible      = WHITE;
                    bang[bk].x            = (dead_x + 5 > 0) ? (uint32_t)(dead_x + 5) : 0;
                    bang[bk].y            = (dead_y >= 2) ? dead_y - 2 : 0;
                    bang[bk].action_image = 1;
                    zw_game_score += 10;
                    BUZZER_PlaySound(BUZZER_SOUND_BANG);
                    break;
                }
            }
        }
    }
        break;

    case ZW_GAME_ZOMBIE_RESET: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_RESET\n");
        game_active = false;
        for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
        zombie[i].visible    = BLACK;
        zombie[i].x          = 200;
        zombie[i].dy         = 0;
        zombie[i].rising     = false;
        zombie[i].rise_ticks = 0;
        }
    }
        break;

    default:
        break;
    }
}