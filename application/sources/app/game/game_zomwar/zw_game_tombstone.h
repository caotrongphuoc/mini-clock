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

#define TOMBSTONES_PER_LANE (2)
#define NUM_TOMBSTONE (10)
#define SIZE_BITMAP_TOMBSTONE_X (8)
#define SIZE_BITMAP_TOMBSTONE_Y (10)
#define TOMBSTONE_SPAWN_INTERVAL (35)
#define TOMBSTONE_LANE_1_X_MIN (65)
#define TOMBSTONE_LANE_1_X_MAX (84)
#define TOMBSTONE_LANE_2_X_MIN (90)
#define TOMBSTONE_LANE_2_X_MAX (109)

typedef struct
{
	uint8_t x;
	uint8_t lane;
	bool active;
} zw_game_tombstone_t;

extern zw_game_tombstone_t tombstone[NUM_TOMBSTONE];

void zw_game_tombstone_handle(ak_msg_t* msg);

#endif //__ZW_GAME_TOMBSTONE_H__