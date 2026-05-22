#include "zw_game_tombstone.h"

zw_game_tombstone_t tombstones[NUM_TOMBSTONES];
static uint8_t tombstone_spawn_timer = 0;
static const uint8_t TOMBSTONE_LANE_1_TMP = 0x1F; // fix tam: bitmask 5 lane nhom 1 (0b11111); sau nay lay tu menu Setting
static const uint8_t TOMBSTONE_LANE_2_TMP = 0x1F; // fix tam: bitmask 5 lane nhom 2 (0b11111); sau nay lay tu menu Setting

void zw_game_tombstone_handle(ak_msg_t* msg) {
    switch (msg->sig) {
    case ZW_GAME_TOMBSTONE_SETUP: {
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_SETUP\n");
        tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL;

        for (uint8_t l = 0; l < NUM_LANES; l++) {
            tombstones[l].x      = (uint8_t)(rand() % 20 + 65);
            tombstones[l].lane   = l;
            tombstones[l].active = (bool)((TOMBSTONE_LANE_1_TMP >> l) & 1);

            tombstones[l + NUM_LANES].x      = (uint8_t)(rand() % 20 + 90);
            tombstones[l + NUM_LANES].lane   = l;
            tombstones[l + NUM_LANES].active = (bool)((TOMBSTONE_LANE_2_TMP >> l) & 1);
        }
    }
        break;

    case ZW_GAME_TOMBSTONE_UPDATE: {
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_UPDATE\n");
        if (tombstone_spawn_timer > 0) {
            tombstone_spawn_timer--;
            return;
        }

        tombstone_spawn_timer = TOMBSTONE_SPAWN_INTERVAL;

        uint8_t tidx = (uint8_t)(rand() % NUM_TOMBSTONES);
        if (tombstones[tidx].active) {
            task_post_pure_msg(ZW_GAME_TOMBSTONE_ID, ZW_GAME_TOMBSTONE_SPAWN);
        }
    }
        break;

    case ZW_GAME_TOMBSTONE_SPAWN: {
        APP_DBG_SIG("ZW_GAME_TOMBSTONE_SPAWN\n");
        uint8_t tidx = (uint8_t)(rand() % NUM_TOMBSTONES);
        if (!tombstones[tidx].active) return;

        for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
            if (zombie[i].visible != WHITE) {
                spawn_zombie_from_tombstone(i, tidx);
                return;
            }
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