#ifndef __ZW_GAME_BULLET_H__
#define __ZW_GAME_BULLET_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "scr_game_zomwar.h"
#include "scr_game_menu.h"

#include "zw_game_gunner.h"

#define MAX_NUM_BULLET       (15)  // So luong vien dan toi da
#define MAX_AXIS_X_BULLET    (128) // Duong bay toi da cua vien dan
#define SIZE_BITMAP_BULLET_X (5)   // Kich thuoc chieu ngang cua vien dan
#define SIZE_BITMAP_BULLET_Y (5)   // Kich thuoc chieu cao cua vien dan
#define STEP_BULLET_AXIS_X   (3)   // Quang duong di chuyen cua vien dan moi tick

typedef struct {
    bool visible;
    uint32_t x, y;
    uint8_t action_image;
} zw_game_bullet_t;

extern zw_game_bullet_t bullet[MAX_NUM_BULLET];
extern uint8_t bullet_count; // so dan active, nam o slot [0..bullet_count)

#endif //__ZW_GAME_BULLET_H__
