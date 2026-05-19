#ifndef __ZW_GAME_TOMBSTONE_H__
#define __ZW_GAME_TOMBSTONE_H__

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
#include "zw_game_bang.h"
#include "zw_game_border.h"
#include "zw_game_zombie.h"

#define TOMBSTONES_PER_LANE         (2)     // So luong tombstones moi lane
#define NUM_TOMBSTONES              (10)    // Tong so luong tombstone trong game
#define SIZE_BITMAP_TOMBSTONE_X     (8)     // Kich thuoc tombstone
#define SIZE_BITMAP_TOMBSTONE_Y     (10)    // Kich thuoc tombstone
#define TOMBSTONE_SPAWN_INTERVAL    (35)    // Khoang cach giua cac bia mo
#define ZOMBIE_RISE_TICKS           (10)    // Zombie mat 10 tick de troi day tu bia bo, 1 tich = 100 ms => 10 tick = 1000 ms = 1s  

typedef struct {           
    bool visible;
    bool rising;
    uint8_t rise_ticks;
    uint32_t x, y;             
} zw_game_tombstone_t;

extern zw_game_tombstone_t tombstones[NUM_TOMBSTONES];

#endif //__ZW_GAME_TOMBSTONE_H__