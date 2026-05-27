#include "zw_game_zombie.h"
#include "app_eeprom.h"

zw_game_zombie_t zombie[NUM_ZOMBIES];
static bool game_active = false; 
const uint8_t ZOMBIE_LEFT_PX[2][SIZE_BITMAP_ZOMBIES_Y] = {
    {9, 8, 8, 9, 6, 9, 9, 9, 9, 8},
    {9, 8, 8, 9, 7, 9, 9, 9, 9, 9},
};

uint8_t zw_game_zombie_speed = ZW_GAME_SETTING_ZOMBIE_SPEED_DEFAULT; 
uint8_t zombie_count = 0; 

void zw_game_zombie_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_ZOMBIE_SETUP: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_SETUP\n");
        game_active = true;
        zw_game_zombie_speed = settingdata.zombie_speed;
        zombie_count = NUM_ZOMBIES_INIT; 
        for (uint8_t i = 0; i < zombie_count; i++) {
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
        break;

    case ZW_GAME_ZOMBIE_RUN: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_RUN\n");
        if(!game_active) return;
        for (uint8_t i = 0; i < zombie_count; i++) {
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
            if (zombie[i].x - (int32_t)zw_game_zombie_speed < -(int32_t)ZOMBIE_MIN_LEFT_OFFSET) {
                zombie[i].x = -(int32_t)ZOMBIE_MIN_LEFT_OFFSET;
            } else {
                zombie[i].x -= (int32_t)zw_game_zombie_speed;
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
        // Tu dong spawn them zombie tu mep phai de luon giu it nhat NUM_ZOMBIES_INIT con
        while (zombie_count < NUM_ZOMBIES_INIT) {
            uint8_t i = zombie_count; // slot trong ke tiep
            zombie[i].x            = (rand() % 39) + 130;
            zombie[i].y            = (rand() % (ZOMBIE_Y_MAX - ZOMBIE_Y_MIN + 1)) + ZOMBIE_Y_MIN;
            zombie[i].visible      = WHITE;
            zombie[i].action_image = rand() % 3 + 1;
            zombie[i].dy           = 0;
            zombie[i].zigzag_timer = rand() % 10 + 5;
            zombie[i].rising       = false;
            zombie[i].rise_ticks   = 0;
            zombie_count++;
        }
    }
        break;

    case ZW_GAME_ZOMBIE_DETONATOR: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_DETONATOR\n");
        for (uint8_t i = 0; i < zombie_count; ) {
            if (zombie[i].rising) { i++; continue; }

            bool killed = false;
            for (uint8_t j = 0; j < bullet_count; j++) {
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

                    // Tieu thu vien dan: xoa dan j bang swap-remove
                    bullet_count--;
                    bullet[j] = bullet[bullet_count];
                    bullet[bullet_count].visible = BLACK;
                    bullet[bullet_count].x       = 0;

                    uint8_t bk       = bang_alloc_slot();
                    bang[bk].visible      = WHITE;
                    bang[bk].x            = (dead_x + 5 > 0) ? (uint32_t)(dead_x + 5) : 0;
                    bang[bk].y            = (dead_y >= 2) ? dead_y - 2 : 0;
                    bang[bk].action_image = 1;
                    zw_game_score += 10;
                    BUZZER_PlaySound(BUZZER_SOUND_BANG);

                    // Xoa zombie i bang swap-remove: dua phan tu cuoi vao slot i
                    zombie_count--;
                    zombie[i] = zombie[zombie_count];
                    zombie[zombie_count].visible = BLACK;
                    killed = true;
                    break;
                }
            }
            if (!killed) i++; // chi tien khi khong xoa; neu xoa thi xet lai phan tu vua hoan doi vao i
        }
    }
        break;

    case ZW_GAME_ZOMBIE_RESET: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_RESET\n");
        game_active = false;
        zombie_count = 0; 
        for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
            zombie[i].visible = BLACK;
        }
    }
        break;

    case ZW_GAME_ZOMBIE_SETUP_MENU: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_SETUP_MENU\n");
        game_active = true;
        zombie_count = 1;
        zombie[0].x = LCD_WIDTH + 3;
        zombie[0].y = AXIS_Y_GUNNER - 10;
        zombie[0].rising = false;
        zombie[0].visible = WHITE;
        zombie[0].action_image = 1;
    }
        break;

    case ZW_GAME_ZOMBIE_RUN_MENU: {
        APP_DBG_SIG("ZW_GAME_ZOMBIE_RUN_MENU\n");
        zombie_count = 1;

        // di chuyen sang trai + doi frame chay
        zombie[0].x -= MENU_ZOMBIE_SPEED;
        zombie[0].visible = WHITE;
        zombie[0].action_image++;
        if (zombie[0].action_image > 3) zombie[0].action_image = 1;

        // ra khoi mep trai -> quay ve mep phai
        if (zombie[0].x < -(int32_t)SIZE_BITMAP_ZOMBIES_X) {
            zombie[0].x = LCD_WIDTH + 3;
            zombie[0].y = AXIS_Y_GUNNER - 10;
        }

        // va cham voi dan -> tao no roi cho zombie ve phai (khong cong diem, khong tieng)
        for (uint8_t j = 0; j < bullet_count; j++) {
            int32_t ax = bullet[j].x;
            int32_t ay = bullet[j].y;
            int32_t mx = zombie[0].x;
            int32_t my = (int32_t)zombie[0].y;
            bool hit = (ax + SIZE_BITMAP_BULLET_X > mx + 12) &&
                       (ax < mx + (int32_t)SIZE_BITMAP_ZOMBIES_X) &&
                       (ay + (int32_t)SIZE_BITMAP_BULLET_Y > my) &&
                       (ay < my + (int32_t)SIZE_BITMAP_ZOMBIES_Y);
            if (hit) {
                int32_t  dead_x = zombie[0].x;
                uint32_t dead_y = zombie[0].y;
                // xoa vien dan (swap-remove)
                bullet_count--;
                bullet[j] = bullet[bullet_count];
                bullet[bullet_count].visible = BLACK;
                bullet[bullet_count].x       = 0;
                // tao hieu ung no
                uint8_t bk = bang_alloc_slot();
                bang[bk].visible      = WHITE;
                bang[bk].x            = (dead_x + 5 > 0) ? (uint32_t)(dead_x + 5) : 0;
                bang[bk].y            = (dead_y >= 2) ? dead_y - 2 : 0;
                bang[bk].action_image = 1;
                // zombie "chet" -> quay ve mep phai
                zombie[0].x = LCD_WIDTH + 3;
                zombie[0].y = AXIS_Y_GUNNER - 10;
                break;
            }
        }
    }
        break;

    default:
        break;
    }
}