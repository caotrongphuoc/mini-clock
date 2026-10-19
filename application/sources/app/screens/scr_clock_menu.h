#ifndef __SCR_CLOCK_MENU_H__
#define __SCR_CLOCK_MENU_H__

#include "screens.h"
#include "mc_clock_time.h"
#include "scr_clock_main.h"
#include "scr_clock_setting_display.h"

// Total number of menu items
#define MENU_ITEM_NUMBER (9)

// Icon position
#define MENU_ICON_X (53)
#define MENU_ICON_Y (17)

// Frame (origin, width, height, corner radius)
#define MENU_FRAME_X (6)
#define MENU_FRAME_Y (13)
#define MENU_FRAME_W (116)
#define MENU_FRAME_H (48) // with the words bottom is 38 else 48
#define MENU_FRAME_R (4)

// Slide distance of the menu animation (px)
#define MENU_ANIMATION_END (64)

typedef enum
{
	MENU_CLOCK,
	MENU_ANALOG,
	MENU_ALARM,
	MENU_STOPWATCH,
	MENU_TIMER,
	MENU_WORLD_CLOCK,
	MENU_CALENDAR,
	MENU_SETTING,
	MENU_EXIT,
} scr_clock_menu_item_t;

#endif //__SCR_CLOCK_MENU_H__
