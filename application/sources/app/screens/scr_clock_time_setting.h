#ifndef __SCR_CLOCK_TIME_SETTING_H__
#define __SCR_CLOCK_TIME_SETTING_H__

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

#include "screens.h"
#include "scr_clock_main.h"

#include "mc_clock_time.h"
#include "mc_clock_date.h"
#include "task_mc_rtc.h"
#include "mc_clock_world.h"
#include "mc_clock_alarm.h"

// Year range supported by the RTC
#define TIME_SETTING_YEAR_MIN (2000)
#define TIME_SETTING_YEAR_MAX (2099)

// Character size of the time row (text size 2)
#define TIME_SETTING_TIME_CHAR_W (12)
#define TIME_SETTING_TIME_CHAR_H (16)

// Character size of the date row (text size 1)
#define TIME_SETTING_INFO_CHAR_W (6)
#define TIME_SETTING_INFO_CHAR_H (8)

// Save button position
#define TIME_SETTING_SAVE_X (110)

typedef enum
{
	TIME_SETTING_YEAR,
	TIME_SETTING_MONTH,
	TIME_SETTING_DATE,
	TIME_SETTING_WEEKDAY,
	TIME_SETTING_HOUR,
	TIME_SETTING_MIN,
	TIME_SETTING_SEC,
	TIME_SETTING_SAVE,
	TIME_SETTING_FIELD_NUMBER,
} scr_clock_time_setting_field_t;

extern view_dynamic_t dyn_view_scr_clock_time_setting;
extern view_screen_t scr_clock_time_setting;
extern void scr_clock_time_setting_handle(ak_msg_t* msg);

#endif //__SCR_CLOCK_TIME_SETTING_H__
