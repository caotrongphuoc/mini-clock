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

#define AXIS_X_BORDER           (0) // Vi tri ranh gioi, 0 - Sat bien man hinh ben trai
// #define AXIS_Y_BORDER_ON        (2)
// #define AXIS_Y_BORDER_UNDER     (52)

typedef struct {
    bool visible;
    uint32_t x, y;
    uint8_t action_image;
} zw_game_border_t;

extern uint16_t zw_game_score;
extern zw_game_border_t border;

#endif //__ZW_GAME_BORDER_H__