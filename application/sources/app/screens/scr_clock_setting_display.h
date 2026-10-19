#ifndef __SCR_CLOCK_SETTING_DISPLAY_H__
#define __SCR_CLOCK_SETTING_DISPLAY_H__

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
#include "scr_clock_setting.h"

// Total number of menu items
#define SETTING_DISPLAY_ITEM_NUMBER (4)

typedef enum
{
	SETTING_DISPLAY_FORMAT,
	SETTING_DISPLAY_COLOR,
	SETTING_DISPLAY_BRIGHT,
	SETTING_DISPLAY_BACK,
} scr_clock_setting_display_item_t;

extern view_dynamic_t dyn_view_scr_clock_setting_display;
extern view_screen_t scr_clock_setting_display;
extern void scr_clock_setting_display_handle(ak_msg_t* msg);

extern uint8_t scr_clock_setting_is_12h_format(void);
extern void scr_clock_setting_display_reset(void);
extern void scr_clock_setting_display_apply(void);

#endif //__SCR_CLOCK_SETTING_DISPLAY_H__
