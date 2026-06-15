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
#define ZOMBIE_WAVE_SPAWN (10)
#define ZOMBIE_SPEED_MAX (6)
#define ZOMBIE_RISE_TICKS (10)
#define MENU_ZOMBIE_SPEED (1)
#define SIZE_BITMAP_ZOMBIES_X (25)
#define SIZE_BITMAP_ZOMBIES_Y (10)
#define ZOMBIE_MIN_LEFT_OFFSET (6)
#define ZOMBIE_HITBOX_LEFT_OFFSET  (12)
#define ZOMBIE_HITBOX_RIGHT_OFFSET (22)
#define ZOMBIE_HITBOX_TOP_OFFSET   (1)
#define ZOMBIE_HITBOX_BOTTOM_OFFSET (9)
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

extern void zw_game_zombie_spawn(uint8_t i);
extern void zw_game_zombie_spawn_rise(uint8_t i, int16_t x, uint8_t y);
extern bool zw_game_zombie_check_hit(uint8_t b, uint8_t z);

#endif //__ZW_GAME_ZOMBIE_H__
