#ifndef __SCR_CLOCK_RINGING_H__
#define __SCR_CLOCK_RINGING_H__

#include "screens.h"
#include "mc_clock_ring.h"
#include "mc_clock_time.h"

// Row positions (title, time, hint)
#define RINGING_TITLE_Y (5)
#define RINGING_TIME_Y (30)
#define RINGING_HINT_Y (52)

/* Handle AC_DISPLAY_SHOW_RINGING (payload mc_clock_ring_req_t): leave the
 * current screen, cancelling its display timers, and show the ringing screen.
 * Called by task_display before the message is dispatched. */
void scr_clock_ringing_enter(ak_msg_t* msg);

#endif //__SCR_CLOCK_RINGING_H__
