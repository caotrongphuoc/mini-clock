#ifndef __ZW_GAME_BANG_H__
#define __ZW_GAME_BANG_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "scr_game_zomwar.h"

// Number of BangS
#define BANG_NUMBER (20)

// Size of Bang (I AND II)
#define BANG_SIZE_BITMAP_I_X (15)
#define BANG_SIZE_BITMAP_I_Y (15)
#define BANG_SIZE_BITMAP_II_X (10)
#define BANG_SIZE_BITMAP_II_Y (10)

// Spawn offset to center bang sprite over hit point
#define BANG_SPAWN_OFFSET_X (5)
#define BANG_SPAWN_OFFSET_Y (2)

typedef struct
{
	uint8_t x, y;
	uint8_t action_image;
	bool visible;
} zw_game_bang_t;

extern zw_game_bang_t bang[BANG_NUMBER];

extern void zw_game_bang_spawn(int16_t x, uint8_t y);

#endif //__ZW_GAME_BANG_H__