#ifndef __SCR_CLOCK_SETTING_H__
#define __SCR_CLOCK_SETTING_H__

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

#include "app_eeprom.h"

#include "screens.h"
#include "scr_clock_setting_display.h"
#include "scr_clock_setting_sound.h"

#include "mc_clock_world.h"
#include "mc_clock_alarm.h"

// Total number of menu items
#define SETTING_ITEM_NUMBER (5)

// Frame layout (origin of first frame and vertical step between frames)
#define SETTING_FRAME_X (2)
#define SETTING_FRAME_Y_1 (2)
#define SETTING_FRAME_STEP (12)

// Frame size (width, height, corner radius)
#define SETTING_FRAME_W (124)
#define SETTING_FRAME_H (11)
#define SETTING_FRAME_R (2)

typedef enum
{
	SETTING_TIME,
	SETTING_DISPLAY,
	SETTING_SOUND,
	SETTING_RESET,
	SETTING_EXIT,
} scr_clock_setting_item_t;

extern view_dynamic_t dyn_view_scr_clock_setting;
extern view_screen_t scr_clock_setting;
extern void scr_clock_setting_handle(ak_msg_t* msg);

#endif //__SCR_CLOCK_SETTING_H__
