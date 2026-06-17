#ifndef __SCR_GAME_SETTING_TOMBSTONE_H__
#define __SCR_GAME_SETTING_TOMBSTONE_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "buzzer.h"

#include "eeprom.h"
#include "app_eeprom.h"

#include "screens.h"
#include "screens_bitmap.h"

#define SETTING_TOMBSTONE_NUMBER (5)
#define SETTING_TOMBSTONE_ITEM_EXIT (SETTING_TOMBSTONE_NUMBER)
#define SETTING_TOMBSTONE_FRAMES_AXIS_X (0)
#define SETTING_TOMBSTONE_FRAMES_AXIS_Y_1 (2)
#define SETTING_TOMBSTONE_FRAMES_STEP (10)
#define SETTING_TOMBSTONE_FRAMES_SIZE_W (123)
#define SETTING_TOMBSTONE_FRAMES_SIZE_H (9)
#define SETTING_TOMBSTONE_FRAMES_SIZE_R (2)

extern view_dynamic_t dyn_view_game_setting_tombstone;
extern view_screen_t scr_game_setting_tombstone;
extern void scr_game_setting_tombstone_handle(ak_msg_t* msg);

#endif //__SCR_GAME_SETTING_TOMBSTONE_H__
