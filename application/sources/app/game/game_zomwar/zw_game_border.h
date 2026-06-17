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

// Wave settings
#define BORDER_WAVE_SCORE_INTERVAL (200)
#define BORDER_WARNING_BLINK_DURATION (30)
#define BORDER_WARNING_BLINK_RATE (3)

// Size of Warning
#define BORDER_SIZE_BITMAP_WARNING_X (16)
#define BORDER_SIZE_BITMAP_WARNING_Y (14)

extern uint16_t zw_game_score;
extern uint16_t wave_last_score;
extern uint8_t wave_warning_timer;
extern uint8_t wave_level;
extern bool wave_warning_active;

#endif //__ZW_GAME_BORDER_H__