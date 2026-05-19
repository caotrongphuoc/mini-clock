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
#include "zw_game_bang.h"
#include "zw_game_border.h"

#define NUM_ZOMBIES                 (50)    // So luong zombie
#define NUM_ZOMBIES_INIT            (5)     // So luong zombie ban dau
#define WAVE_SPAWN_COUNT            (8)     // So luong zombie them vao moi wave
#define WAVE_SCORE_INTERVAL         (200)   // So diem can dat de spawn 1 wave 
#define ZOMBIE_SPEED_MAX            (6)     // Toc do toi da cua 1 zombie = 6px
#define SIZE_BITMAP_ZOMBIES_X       (25)    // Kich thuoc chieu ngang cua zombie
#define SIZE_BITMAP_ZOMBIES_Y       (10)    // Kich thuoc chieu cao cua zombie
#define ZOMBIE_MIN_LEFT_OFFSET      (6)     // 
#define ZOMBIE_Y_MIN                (2)     // Gioi han di chuyen cua zombie o vi tri tren man hinh lcd
#define ZOMBIE_Y_MAX                (42)    // Gioi han di chuyen cua zombie o vi tri duoi man hinh lcd
#define WARNING_BLINK_DURATION      (30)    // Thoi gian canh bao
#define WARNING_BLINK_RATE          (3)     // Nhap nhay moi 3 giay
#define SIZE_BITMAP_WARNING_X       (16)    // Kich thuoc canh bao
#define SIZE_BITMAP_WARNING_Y       (14)    // Kich thuoc canh bao

typedef struct {
    int8_t dy;            
    uint8_t zigzag_timer;   
    uint8_t action_image;
    bool visible;
    bool rising;
    int32_t x;
    uint32_t y;          
    uint8_t rise_ticks;   
} zw_game_zombie_t;

typedef struct {
    uint8_t x;
    uint8_t lane;
    bool active;
} tombstone_t;

extern tombstone_t tombstones[NUM_TOMBSTONES];
extern const uint8_t ZOMBIE_LEFT_PX[2][SIZE_BITMAP_ZOMBIES_Y];
extern zw_game_zombie_t zombie[NUM_ZOMBIES];
extern uint32_t last_wave_score;
extern uint8_t wave_warning_timer;    
extern bool wave_warning_active;
extern uint8_t wave_level;

#endif //__ZW_GAME_ZOMBIE_H__
