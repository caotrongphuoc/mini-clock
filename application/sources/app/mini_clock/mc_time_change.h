#ifndef __MC_TIME_CHANGE_H__
#define __MC_TIME_CHANGE_H__

#include "screens.h"
#include "task_mc_rtc.h"

#define scr_width 128
#define scr_height 64
#define total_time_zone 10

extern uint8_t current_time_zone;   // Make it a variable, not #define

extern uint8_t global_hour;
extern uint8_t global_minute;
extern uint8_t global_day;
extern uint8_t global_month;

#endif //__MC_TIME_CHANGE_H__