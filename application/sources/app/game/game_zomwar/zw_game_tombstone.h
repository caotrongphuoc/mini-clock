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

#include "zw_game_zombie.h"
#include "zw_game_car.h"
#include "scr_game_zomwar.h"

#define TOMBSTONES_PER_LANE      (2)    // So bia mo toi da moi lane
#define NUM_TOMBSTONES           (10)   // Tong so luong bia mo trong game = 2 bia mo * 5 lane
#define SIZE_BITMAP_TOMBSTONE_X  (8)    // Kich thuoc bia mo theo chieu ngang
#define SIZE_BITMAP_TOMBSTONE_Y  (10)   // Kich thuoc bia mo theo chieu cao
#define TOMBSTONE_SPAWN_INTERVAL (35)   // Thoi gian spawn zombie tu bia mo
#define ZOMBIE_RISE_TICKS        (10)   // Thoi gian zombie troi len tu bia mo

typedef struct {
    uint8_t x;
    uint8_t lane;
    bool    active;
} zw_game_tombstone_t;

extern zw_game_tombstone_t tombstones[NUM_TOMBSTONES];

void zw_game_tombstone_handle(ak_msg_t* msg);

#endif //__ZW_GAME_TOMBSTONE_H__