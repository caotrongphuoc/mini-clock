#ifndef __SCR_CLOCK_MAIN_H__
#define __SCR_CLOCK_MAIN_H__

#include "screens.h"

#include "mc_clock_time.h"

#define SCR_CLOCK_MAIN_TIME_X (1)
#define SCR_CLOCK_MAIN_TIME_Y (14)
#define SCR_CLOCK_MAIN_SEC_X (17)
#define SCR_CLOCK_MAIN_SEC_Y (34)
#define SCR_CLOCK_MAIN_CAL_X (60)
#define SCR_CLOCK_MAIN_CAL_MONTH_Y (0)
#define SCR_CLOCK_MAIN_CAL_WEEKDAY_Y (10)
#define SCR_CLOCK_MAIN_CAL_DAY_Y (19)
#define SCR_CLOCK_MAIN_CAL_COL_STEP (9)
#define SCR_CLOCK_MAIN_CAL_ROW_STEP (7)
#define SCR_CLOCK_MAIN_DATE_X (24)
#define SCR_CLOCK_MAIN_DATE_Y (48)
#define SCR_CLOCK_MAIN_WEEKDAY_X (52)
#define SCR_CLOCK_MAIN_WEEKDAY_Y (0)
#define SCR_CLOCK_MAIN_TIME_TEXT_SIZE (2)
#define SCR_CLOCK_MAIN_INFO_TEXT_SIZE (1)

const char* scr_clock_main_weekday_text(uint8_t weekday);

#endif //__SCR_CLOCK_MAIN_H__
