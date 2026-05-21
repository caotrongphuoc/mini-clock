#ifndef __ZW_GAME_BANG_H__
#define __ZW_GAME_BANG_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "scr_game_zomwar.h"

#define NUM_BANG                    (20)
#define SIZE_BITMAP_BANG_I_X        (15)
#define SIZE_BITMAP_BANG_I_Y        (15)
#define SIZE_BITMAP_BANG_II_X       (10)
#define SIZE_BITMAP_BANG_II_Y       (10)

typedef struct {
    uint32_t x, y;
    uint8_t action_image;
    bool visible;
} zw_game_bang_t;

extern zw_game_bang_t bang[NUM_BANG];
uint8_t bang_alloc_slot(void);

#endif //__ZW_GAME_BANG_H__