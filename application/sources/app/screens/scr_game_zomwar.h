#ifndef __SCR_GAME_ZOMWAR_H__
#define __SCR_GAME_ZOMWAR_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_bsp.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "eeprom.h"
#include "app_eeprom.h"

#include "buzzer.h"

#include "scr_idle.h"
#include "screens_bitmap.h"
#include "screens.h"

#include "zw_game_gunner.h"
#include "zw_game_bullet.h"
#include "zw_game_zombie.h"
#include "zw_game_car.h"
#include "zw_game_tombstone.h"
#include "zw_game_bang.h"
#include "zw_game_border.h"

#define GAME_OFF (0)  // Trang thai LCD khi khong choi game
#define GAME_PLAY (1) // Trang thai LCD khi choi game
#define GAME_OVER (2) // Trang thai LCD khi thua

extern view_dynamic_t dyn_view_zomwar;
extern view_screen_t scr_game_zomwar;
extern void scr_game_zomwar_handle(ak_msg_t *msg);

extern void zw_game_gunner_display();
extern void zw_game_bullet_display();
extern void zw_game_zombie_display();
extern void zw_game_bang_display();

#endif //__SCR_GAME_ZOMWAR_H__