#ifndef __SCR_CLOCK_MENU_H__
#define __SCR_CLOCK_MENU_H__

#include "screens.h"
#include "mc_clock_time.h"
#include "scr_clock_main.h"
#include "scr_clock_setting_display.h"
#include "screens.h"

#define SCR_CLOCK_MENU_ITEM_NUMBER (8)
#define SCR_CLOCK_MENU_ICON_X (53)
#define SCR_CLOCK_MENU_ICON_Y (17)
#define SCR_CLOCK_MENU_FRAME_X (6)
#define SCR_CLOCK_MENU_FRAME_Y (13)
#define SCR_CLOCK_MENU_FRAME_W (116)
#define SCR_CLOCK_MENU_FRAME_H (48) //with the words bottom is 38 else 48
#define SCR_CLOCK_MENU_FRAME_R (4)

// Animation variables
#define MENU_ANIMATION_STEP 8
#define MENU_ANIMATION_END 64

typedef enum
{
	SCR_CLOCK_MENU_CLOCK,
	SCR_CLOCK_MENU_ALARM,
	SCR_CLOCK_MENU_STOPWATCH,
	SCR_CLOCK_MENU_TIMER,
	SCR_CLOCK_MENU_WORLD_CLOCK,
	SCR_CLOCK_MENU_CALENDAR,
	SCR_CLOCK_MENU_SETTING,
	SCR_CLOCK_MENU_EXIT,
} scr_clock_menu_item_t;

#endif //__SCR_CLOCK_MENU_H__
