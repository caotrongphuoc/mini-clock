#ifndef __ZW_GAME_GUNNER_H__
#define __ZW_GAME_GUNNER_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "scr_game_zomwar.h"

#define GUNNER_STEP_AXIS_Y (10)
#define GUNNER_SIZE_BITMAP_X (25)
#define GUNNER_SIZE_BITMAP_Y (10)
#define GUNNER_AXIS_X (14)
#define GUNNER_AXIS_Y (52)
#define GUNNER_AXIS_Y_MIN (12)
#define GUNNER_AXIS_Y_MAX (52)

typedef struct
{
	uint8_t x;
	uint8_t y;
	uint8_t action_image;
	bool visible;
} zw_game_gunner_t;

extern zw_game_gunner_t gunner;

#endif //__ZW_GAME_GUNNER_H__
