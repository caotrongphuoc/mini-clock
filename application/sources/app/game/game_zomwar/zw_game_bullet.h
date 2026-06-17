#ifndef __ZW_GAME_BULLET_H__
#define __ZW_GAME_BULLET_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "scr_game_zomwar.h"
#include "scr_game_menu.h"

#include "zw_game_gunner.h"

// Number of Bullets
#define BULLET_NUMBER (15)

// Size of Bullet bitmap
#define BULLET_SIZE_BITMAP_X (5)
#define BULLET_SIZE_BITMAP_Y (5)

// Right edge (despawn when reaching this x)
#define BULLET_MAX_AXIS_X (128)

// Horizontal movement step per RUN signal
#define BULLET_STEP_AXIS_X (3)

// Spawn offset relative to gunner position
#define BULLET_SPAWN_OFFSET_X (22)
#define BULLET_SPAWN_OFFSET_Y (8)

typedef struct
{
	bool visible;
	uint8_t x, y;
} zw_game_bullet_t;

extern zw_game_bullet_t bullet[BULLET_NUMBER];

#endif //__ZW_GAME_BULLET_H__
