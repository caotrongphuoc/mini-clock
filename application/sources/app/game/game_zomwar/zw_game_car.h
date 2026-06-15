#ifndef __ZW_GAME_CAR_H__
#define __ZW_GAME_CAR_H__

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

#include "zw_game_zombie.h"

#define AXIS_X_CAR (0)
#define NUM_LANES (5)
#define CAR_SPEED (4)
#define SIZE_BITMAP_CAR_X (12)
#define SIZE_BITMAP_CAR_Y (10)
#define CAR_HIT_RANGE_Y (8)
#define LANE_Y {2, 12, 22, 32, 42}

typedef struct
{
    bool visible;
    bool running;
    uint8_t x, y;
    uint8_t lane;
    uint8_t action_image;
} zw_game_car_t;

extern zw_game_car_t car[NUM_LANES];

extern bool zw_game_car_check_hit(uint8_t c, uint8_t z);

#endif //__ZW_GAME_CAR_H__
