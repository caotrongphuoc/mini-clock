#ifndef __SCR_CLOCK_STOPWATCH_H__
#define __SCR_CLOCK_STOPWATCH_H__

#include "screens.h"
#include "ak.h"
#include "app_dbg.h"
#include "task_list.h"
#include "message.h"
#include <stdio.h>

typedef enum
{
    SCR_CLOCK_STOPWATCH_DISPLAY,
    SCR_CLOCK_STOPWATCH_START,
    SCR_CLOCK_STOPWATCH_PAUSE,
    SCR_CLOCK_STOPWATCH_RESET,
    SCR_CLOCK_STOPWATCH_LAP,
    SCR_CLOCK_STOPWATCH_BACK,
} scr_clock_stopwatch_item_t;


/* Called every 10ms */
void stopwatch_background_update(void);


#endif // __SCR_CLOCK_STOPWATCH_H__