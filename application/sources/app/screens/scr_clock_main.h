#ifndef __SCR_CLOCK_MAIN_H__
#define __SCR_CLOCK_MAIN_H__

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
#include "scr_clock_setting_display.h"

#include "mc_clock_time.h"

// Content area (1 px margin on each side)
#define MAIN_CONTENT_X (1)
#define MAIN_CONTENT_W (LCD_WIDTH - (MAIN_CONTENT_X * 2))

// Time text position
#define MAIN_TIME_X (16)
#define MAIN_TIME_Y (27)

// Weekday row (7 segments, Mon..Sun)
#define MAIN_WEEKDAY_AREA_X (MAIN_CONTENT_X)
#define MAIN_WEEKDAY_COUNT (7)
#define MAIN_WEEKDAY_SEG_W (MAIN_CONTENT_W / MAIN_WEEKDAY_COUNT)
#define MAIN_WEEKDAY_TEXT_OFFSET_X (3)
#define MAIN_WEEKDAY_ROW_Y (6)

// Weekday highlight box
#define MAIN_WEEKDAY_BOX_Y (4)
#define MAIN_WEEKDAY_BOX_W (MAIN_WEEKDAY_SEG_W)
#define MAIN_WEEKDAY_BOX_H (12)
#define MAIN_SELECT_PAD_X (2)
#define MAIN_SELECT_PAD_Y (1)

// Time frame (origin, width, height, corner radius)
#define MAIN_TIME_FRAME_X (6)
#define MAIN_TIME_FRAME_Y (18)
#define MAIN_TIME_FRAME_W (116)
#define MAIN_TIME_FRAME_H (32)
#define MAIN_TIME_FRAME_R (4)

// Date text position
#define MAIN_DATE_TEXT_X (34)
#define MAIN_DATE_TEXT_Y (54)

// Text sizes (time, info)
#define MAIN_TIME_TEXT_SIZE (2)
#define MAIN_INFO_TEXT_SIZE (1)

extern view_dynamic_t dyn_view_scr_clock_main;
extern view_screen_t scr_clock_main;
extern void scr_clock_main_handle(ak_msg_t* msg);

extern const char* scr_clock_main_weekday_short_text(uint8_t weekday);
extern void scr_clock_main_format_time(char* buffer, rtc_time_t* time, uint8_t use_12h);
extern void scr_clock_main_format_date(char* buffer, rtc_date_t* date);
extern void scr_clock_main_draw_weekdays(uint8_t weekday, uint8_t inverted);

#endif //__SCR_CLOCK_MAIN_H__
