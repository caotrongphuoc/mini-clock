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

#define STEP_GUNNER_AXIS_Y (10)
#define SIZE_BITMAP_GUNNER_X (25)
#define SIZE_BITMAP_GUNNER_Y (10)
#define AXIS_X_GUNNER (14)
#define AXIS_Y_GUNNER (52)
#define AXIS_Y_GUNNER_MIN (12)
#define AXIS_Y_GUNNER_MAX (52)

typedef struct
{
    bool visible;
    uint32_t x;
    uint8_t y;
    uint8_t action_image;
} zw_game_gunner_t;

extern zw_game_gunner_t gunner;

#endif //__ZW_GAME_GUNNER_H__
