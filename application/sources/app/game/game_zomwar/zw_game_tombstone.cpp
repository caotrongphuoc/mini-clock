#include "zw_game_tombstone.h"
#include "app_eeprom.h"

zw_game_tombstone_t tombstones[NUM_TOMBSTONES];
static const uint8_t lane_y_arr[NUM_LANES] = LANE_Y;
static uint8_t tombstone_spawn_timer = 0;

void zw_game_tombstone_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_TOMBSTONE_SETUP: {
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_SETUP\n");
        tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL;

        for (uint8_t l = 0; l < NUM_LANES; l++) {
            tombstones[l].x      = (uint8_t)(rand() % 20 + 65);
            tombstones[l].lane   = l;
            tombstones[l].active = (bool)((settingdata.tombstone_lane_1 >> l) & 1);

            tombstones[l + NUM_LANES].x      = (uint8_t)(rand() % 20 + 90);
            tombstones[l + NUM_LANES].lane   = l;
            tombstones[l + NUM_LANES].active = (bool)((settingdata.tombstone_lane_2 >> l) & 1);
        }
    }
        break;

    case ZW_GAME_TOMBSTONE_SPAWN: {
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_SPAWN\n");
        // chi spawn sau moi TOMBSTONE_SPAWN_INTERVAL tick 
        if (tombstone_spawn_timer > 0) {
            tombstone_spawn_timer--;
            return;
        }
        tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL;
        // chon ngau nhien 1 bia mo; neu dang bat thi cho zombie troi len tu do 
        uint8_t tidx = (uint8_t)(rand() % NUM_TOMBSTONES);
        if (!tombstones[tidx].active) return;

        // spawn tai slot trong ke tiep (zombie_count) neu con cho 
        if (zombie_count < NUM_ZOMBIES) {
            uint8_t i = zombie_count;
            zombie[i].x            = tombstones[tidx].x;
            zombie[i].y            = (uint32_t)lane_y_arr[tombstones[tidx].lane] + SIZE_BITMAP_TOMBSTONE_Y;
            zombie[i].visible      = WHITE;
            zombie[i].action_image = 1;
            zombie[i].dy           = 0;
            zombie[i].zigzag_timer = 0;
            zombie[i].rising       = true;
            zombie[i].rise_ticks   = ZOMBIE_RISE_TICKS;
            zombie_count++;
        }
    }
        break;

    case ZW_GAME_TOMBSTONE_RESET: {
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_RESET\n");
        tombstone_spawn_timer = 0;

        for (uint8_t i = 0; i < NUM_TOMBSTONES; i++) {
            tombstones[i].x      = 0;
            tombstones[i].lane   = 0;
            tombstones[i].active = false;
        }
    }
        break;

    default:
        break;
    }
}