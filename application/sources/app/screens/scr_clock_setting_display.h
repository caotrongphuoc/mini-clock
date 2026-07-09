#ifndef __SCR_CLOCK_SETTING_DISPLAY_H__
#define __SCR_CLOCK_SETTING_DISPLAY_H__

#include "screens.h"

#define SCR_CLOCK_SETTING_DISPLAY_ITEM_NUMBER (2)

typedef enum
{
	SCR_CLOCK_SETTING_DISPLAY_FORMAT,
	SCR_CLOCK_SETTING_DISPLAY_BACK,
} scr_clock_setting_display_item_t;

uint8_t scr_clock_setting_is_12h_format(void);

#endif //__SCR_CLOCK_SETTING_DISPLAY_H__
