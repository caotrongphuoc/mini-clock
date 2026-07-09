#ifndef __SCR_CLOCK_MAIN_H__
#define __SCR_CLOCK_MAIN_H__

#include "screens.h"

#include "mc_clock_time.h"

#define SCR_CLOCK_MAIN_TIME_X (16)
#define SCR_CLOCK_MAIN_TIME_Y (24)
#define SCR_CLOCK_MAIN_DATE_X (24)
#define SCR_CLOCK_MAIN_DATE_Y (48)
#define SCR_CLOCK_MAIN_WEEKDAY_X (52)
#define SCR_CLOCK_MAIN_WEEKDAY_Y (0)
#define SCR_CLOCK_MAIN_WEEKDAY_ROW_X (9)
#define SCR_CLOCK_MAIN_WEEKDAY_ROW_Y (6)
#define SCR_CLOCK_MAIN_DATE_TEXT_X (34)
#define SCR_CLOCK_MAIN_DATE_TEXT_Y (56)
#define SCR_CLOCK_MAIN_TIME_TEXT_SIZE (2)
#define SCR_CLOCK_MAIN_INFO_TEXT_SIZE (1)

const char* scr_clock_main_weekday_text(uint8_t weekday);

#endif //__SCR_CLOCK_MAIN_H__
