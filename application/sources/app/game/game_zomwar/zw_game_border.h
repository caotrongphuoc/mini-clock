#ifndef __ZW_GAME_BORDER_H__
#define __ZW_GAME_BORDER_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "scr_game_zomwar.h"

#include "zw_game_zombie.h"
#include "zw_game_car.h"

#define AXIS_X_BORDER (0)
#define AXIS_Y_BORDER_ON (2)
#define AXIS_Y_BORDER_UNDER (52)
#define WAVE_SCORE_INTERVAL (200)
#define WAVE_SPAWN_COUNT (10)
#define WARNING_BLINK_DURATION (30)
#define WARNING_BLINK_RATE (3)
#define SIZE_BITMAP_WARNING_X (16)
#define SIZE_BITMAP_WARNING_Y (14)

typedef struct
{
    bool visible;
    uint32_t x, y;
    uint8_t action_image;
} zw_game_border_t;

extern uint32_t zw_game_score;
extern zw_game_border_t border;

extern uint32_t wave_last_score;
extern uint8_t wave_warning_timer;
extern bool wave_warning_active;
extern uint8_t wave_level;

#endif //__ZW_GAME_BORDER_H__