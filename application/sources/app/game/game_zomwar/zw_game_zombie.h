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

/*---------------------------------------------------------------------------*/
/*  Constants                                                                */
/*---------------------------------------------------------------------------*/
#define NUM_ZOMBIES              (30)   // So luong zombie 
#define NUM_ZOMBIES_INIT         (5)    // So luong zombie xuat hien tu ben phai man hinh  
#define WAVE_SPAWN_COUNT         (8)    // So luong zombie xuat hien trong moi wave
#define WAVE_SCORE_INTERVAL      (200)  // So diem can dat duoc de xuat hien wave
#define ZOMBIE_SPEED_MAX         (6)    // Toc do toi da cua zombie

#define SIZE_BITMAP_ZOMBIES_X    (25)   // Kich thuoc theo chieu ngang cua zombie
#define SIZE_BITMAP_ZOMBIES_Y    (10)   // Kich thuoc theo chieu cao cua zombie
#define ZOMBIE_MIN_LEFT_OFFSET   (6)    // Khoang cach toi thieu zombie co the di chuyen toi tinh tu ben trai

#define ZOMBIE_Y_MIN             (2)    // zombie không lên cao hơn y=2 (giới hạn trên màn hình)
#define ZOMBIE_Y_MAX             (42)   // zombie không xuống thấp hơn y=42 (giới hạn dưới màn hình, màn hình OLED thường 64px

#define WARNING_BLINK_DURATION   (30)   // hiệu ứng warning nhấp nháy kéo dài 30 tick
#define WARNING_BLINK_RATE       (3)    // cứ 3 tick đổi trạng thái nhấp nháy 1 lần

#define SIZE_BITMAP_WARNING_X    (16)   // icon warning rộng 16 pixel
#define SIZE_BITMAP_WARNING_Y    (14)   // icon warning cao 14 pixel

typedef struct {
    int32_t  x;
    uint32_t y;
    int8_t   dy;
    uint8_t  zigzag_timer;
    uint8_t  action_image;
    bool     visible;
    bool     rising;
    uint8_t  rise_ticks;
} zw_game_zombie_t;

extern const uint8_t    ZOMBIE_LEFT_PX[2][SIZE_BITMAP_ZOMBIES_Y];
extern zw_game_zombie_t zombie[NUM_ZOMBIES];
extern uint32_t         last_wave_score;
extern uint8_t          wave_warning_timer;
extern bool             wave_warning_active;
extern uint8_t          wave_level;

void spawn_zombie_from_tombstone(uint8_t i, uint8_t tidx);

#endif //__ZW_GAME_ZOMBIE_H__
