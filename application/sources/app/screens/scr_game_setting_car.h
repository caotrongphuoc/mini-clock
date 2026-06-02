#ifndef __SCR_GAME_SETTING_CAR_H__
#define __SCR_GAME_SETTING_CAR_H__

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

#define CAR_POS_NUM_CARS				(5)
#define CAR_POS_ITEM_EXIT				(CAR_POS_NUM_CARS)
#define CAR_POS_FRAMES_AXIS_X			(0)
#define CAR_POS_FRAMES_AXIS_Y_1			(2)
#define CAR_POS_FRAMES_STEP				(10)
#define CAR_POS_FRAMES_SIZE_W			(123)
#define CAR_POS_FRAMES_SIZE_H			(9)
#define CAR_POS_FRAMES_SIZE_R			(2)

extern view_dynamic_t dyn_view_game_setting_car;
extern view_screen_t scr_game_setting_car;
extern void scr_game_setting_car_handle(ak_msg_t* msg);

#endif //__SCR_GAME_SETTING_CAR_H__
