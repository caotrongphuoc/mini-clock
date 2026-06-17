#ifndef __ZW_GAME_TOMBSTONE_H__
#define __ZW_GAME_TOMBSTONE_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "zw_game_zombie.h"
#include "zw_game_car.h"
#include "scr_game_zomwar.h"

// Number of Tombstones (slots per lane, total array length)
#define TOMBSTONE_PER_LANE (2)
#define TOMBSTONE_NUMBER (10)

// Size of Tombstone bitmap
#define TOMBSTONE_SIZE_BITMAP_X (8)
#define TOMBSTONE_SIZE_BITMAP_Y (10)

// Interval between two zombie-from-tombstone spawns (ticks)
#define TOMBSTONE_SPAWN_INTERVAL (35)

// X range for tombstone group 1 (left)
#define TOMBSTONE_LANE_1_X_MIN (65)
#define TOMBSTONE_LANE_1_X_MAX (84)

// X range for tombstone group 2 (right)
#define TOMBSTONE_LANE_2_X_MIN (90)
#define TOMBSTONE_LANE_2_X_MAX (109)

typedef struct
{
	uint8_t x;
	uint8_t lane;
	bool active;
} zw_game_tombstone_t;

extern zw_game_tombstone_t tombstone[TOMBSTONE_NUMBER];

void zw_game_tombstone_handle(ak_msg_t* msg);

#endif //__ZW_GAME_TOMBSTONE_H__