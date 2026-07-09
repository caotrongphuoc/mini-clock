#ifndef __SCR_CLOCK_MAIN_H__
#define __SCR_CLOCK_MAIN_H__

#include "screens.h"

#include "mc_clock_time.h"

#define SCR_CLOCK_MAIN_CONTENT_X (1)
#define SCR_CLOCK_MAIN_CONTENT_W (LCD_WIDTH - (SCR_CLOCK_MAIN_CONTENT_X * 2))

#define SCR_CLOCK_MAIN_TIME_X (16)
#define SCR_CLOCK_MAIN_TIME_Y (27)
#define SCR_CLOCK_MAIN_WEEKDAY_AREA_X (SCR_CLOCK_MAIN_CONTENT_X)
#define SCR_CLOCK_MAIN_WEEKDAY_COUNT (7)
#define SCR_CLOCK_MAIN_WEEKDAY_SEG_W (SCR_CLOCK_MAIN_CONTENT_W / SCR_CLOCK_MAIN_WEEKDAY_COUNT)
#define SCR_CLOCK_MAIN_WEEKDAY_TEXT_OFFSET_X (3)
#define SCR_CLOCK_MAIN_WEEKDAY_ROW_X (SCR_CLOCK_MAIN_WEEKDAY_AREA_X + SCR_CLOCK_MAIN_WEEKDAY_TEXT_OFFSET_X)
#define SCR_CLOCK_MAIN_WEEKDAY_ROW_Y (6)
#define SCR_CLOCK_MAIN_WEEKDAY_BOX_Y (4)
#define SCR_CLOCK_MAIN_WEEKDAY_STEP (SCR_CLOCK_MAIN_WEEKDAY_SEG_W)
#define SCR_CLOCK_MAIN_WEEKDAY_TEXT_W (12)
#define SCR_CLOCK_MAIN_WEEKDAY_TEXT_H (8)
#define SCR_CLOCK_MAIN_SELECT_PAD_X (2)
#define SCR_CLOCK_MAIN_SELECT_PAD_Y (1)
#define SCR_CLOCK_MAIN_WEEKDAY_BOX_W (SCR_CLOCK_MAIN_WEEKDAY_SEG_W)
#define SCR_CLOCK_MAIN_WEEKDAY_BOX_H (12)
#define SCR_CLOCK_MAIN_TIME_FRAME_X (6)
#define SCR_CLOCK_MAIN_TIME_FRAME_Y (18)
#define SCR_CLOCK_MAIN_TIME_FRAME_W (116)
#define SCR_CLOCK_MAIN_TIME_FRAME_H (32)
#define SCR_CLOCK_MAIN_TIME_FRAME_R (4)
#define SCR_CLOCK_MAIN_DATE_TEXT_X (34)
#define SCR_CLOCK_MAIN_DATE_TEXT_Y (54)

#define SCR_CLOCK_MAIN_TIME_TEXT_SIZE (2)
#define SCR_CLOCK_MAIN_INFO_TEXT_SIZE (1)

const char* scr_clock_main_weekday_short_text(uint8_t weekday);
void scr_clock_main_format_time(char* buffer, rtc_time_t* time);
void scr_clock_main_format_date(char* buffer, rtc_date_t* date);
void scr_clock_main_draw_weekdays(uint8_t weekday, uint8_t inverted);

#endif //__SCR_CLOCK_MAIN_H__
