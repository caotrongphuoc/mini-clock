#ifndef __SCR_CLOCK_ALARM_H__
#define __SCR_CLOCK_ALARM_H__
#include "screens.h"

#define SCR_CLOCK_ALARM_MAX         (5)     // Maximum number of alarms

typedef enum
{
    SCR_CLOCK_ALARM_LIST,
    SCR_CLOCK_ALARM_ADD,
    SCR_CLOCK_ALARM_EDIT,
    SCR_CLOCK_ALARM_DELETE,
    SCR_CLOCK_ALARM_BACK,
} scr_clock_alarm_item_t;

#endif //__SCR_CLOCK_ALARM_H__