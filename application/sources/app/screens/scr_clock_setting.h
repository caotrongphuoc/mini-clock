#ifndef __SCR_CLOCK_SETTING_H__
#define __SCR_CLOCK_SETTING_H__

#include "screens.h"

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

#endif //__SCR_CLOCK_SETTING_H__
