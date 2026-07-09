#ifndef __SCR_CLOCK_MAIN_H__
#define __SCR_CLOCK_MAIN_H__

#include "screens.h"

#include "mc_clock_time.h"

#define SCR_CLOCK_MAIN_TIME_X (16)
#define SCR_CLOCK_MAIN_TIME_Y (24)
#define SCR_CLOCK_MAIN_WEEKDAY_ROW_X (6)
#define SCR_CLOCK_MAIN_WEEKDAY_ROW_Y (6)
#define SCR_CLOCK_MAIN_WEEKDAY_STEP (18)
#define SCR_CLOCK_MAIN_WEEKDAY_BOX_W (14)
#define SCR_CLOCK_MAIN_WEEKDAY_BOX_H (10)
#define SCR_CLOCK_MAIN_TIME_FRAME_X (7)
#define SCR_CLOCK_MAIN_TIME_FRAME_Y (18)
#define SCR_CLOCK_MAIN_TIME_FRAME_W (114)
#define SCR_CLOCK_MAIN_TIME_FRAME_H (32)
#define SCR_CLOCK_MAIN_TIME_FRAME_R (4)
#define SCR_CLOCK_MAIN_DATE_TEXT_X (34)
#define SCR_CLOCK_MAIN_DATE_TEXT_Y (56)

#define SCR_CLOCK_MAIN_TIME_TEXT_SIZE (2)
#define SCR_CLOCK_MAIN_INFO_TEXT_SIZE (1)

const char* scr_clock_main_weekday_short_text(uint8_t weekday);
void scr_clock_main_format_time(char* buffer, rtc_time_t* time);
void scr_clock_main_format_date(char* buffer, rtc_date_t* date);

#endif //__SCR_CLOCK_MAIN_H__
