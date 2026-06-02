#ifndef __ZW_GAME_ZOMBIE_H__
#define __ZW_GAME_ZOMBIE_H__

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

#include "zw_game_bullet.h"
#include "zw_game_gunner.h"

#define NUM_ZOMBIES (20)
#define NUM_ZOMBIES_INIT (6)
#define ZOMBIE_SPEED_MAX (6)
#define MENU_ZOMBIE_SPEED (1)
#define SIZE_BITMAP_ZOMBIES_X (25)
#define SIZE_BITMAP_ZOMBIES_Y (10)
#define ZOMBIE_MIN_LEFT_OFFSET (6)
#define ZOMBIE_Y_MIN (2)
#define ZOMBIE_Y_MAX (42)

typedef struct
{
    int16_t x;
    uint8_t y;
    int8_t dy;
    uint8_t zigzag_timer;
    uint8_t action_image;
    bool visible;
    bool rising;
    uint8_t rise_ticks;
} zw_game_zombie_t;

extern zw_game_zombie_t zombie[NUM_ZOMBIES];
extern uint8_t zw_game_zombie_speed;

#endif //__ZW_GAME_ZOMBIE_H__
