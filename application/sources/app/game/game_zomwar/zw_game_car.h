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

// Number of Lanes (one rescue car slot per lane)
#define CAR_LANE_NUMBER (5)

// Parked X position (left edge, before activation)
#define CAR_AXIS_X (0)

// Y position of each lane (used for parking and rendering)
#define CAR_LANE_Y {2, 12, 22, 32, 42 }

// Size of Car bitmap
#define CAR_SIZE_BITMAP_X (12)
#define CAR_SIZE_BITMAP_Y (10)

// Horizontal speed once the car is running
#define CAR_SPEED (4)

// Vertical hit / activation range (max |zombie.y - car.y|)
#define CAR_HIT_RANGE_Y (8)

typedef struct
{
	bool visible;
	bool running;
	uint8_t x, y;
	uint8_t lane;
	uint8_t action_image;
} zw_game_car_t;

extern zw_game_car_t car[CAR_LANE_NUMBER];
extern const uint8_t lane_y[CAR_LANE_NUMBER];

extern bool zw_game_car_check_hit(uint8_t c, uint8_t z);
extern int8_t zw_game_car_find_nearest(uint8_t zy);

#endif //__ZW_GAME_CAR_H__
