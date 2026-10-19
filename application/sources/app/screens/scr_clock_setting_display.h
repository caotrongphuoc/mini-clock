#ifndef __SCR_CLOCK_SETTING_DISPLAY_H__
#define __SCR_CLOCK_SETTING_DISPLAY_H__

#include "screens.h"

// Total number of menu items
#define SETTING_DISPLAY_ITEM_NUMBER (4)

typedef enum
{
	SETTING_DISPLAY_FORMAT,
	SETTING_DISPLAY_COLOR,
	SETTING_DISPLAY_BRIGHT,
	SETTING_DISPLAY_BACK,
} scr_clock_setting_display_item_t;

uint8_t scr_clock_setting_is_12h_format(void);
void scr_clock_setting_display_reset(void);
void scr_clock_setting_display_apply(void);

#endif //__SCR_CLOCK_SETTING_DISPLAY_H__
