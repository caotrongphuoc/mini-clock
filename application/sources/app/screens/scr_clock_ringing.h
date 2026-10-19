#ifndef __SCR_CLOCK_RINGING_H__
#define __SCR_CLOCK_RINGING_H__

#include "screens.h"
#include "mc_clock_alarm.h"
#include "mc_clock_time.h"

#define SCR_CLOCK_RINGING_TITLE_Y (8)
#define SCR_CLOCK_RINGING_TIME_Y  (30)
#define SCR_CLOCK_RINGING_HINT_Y  (52)

/* Leave the current screen (cancelling its display timers) and show the
 * ringing screen. Called by task_display on AC_DISPLAY_SHOW_ALARM_RINGING. */
void scr_clock_ringing_enter(void);

#endif //__SCR_CLOCK_RINGING_H__
