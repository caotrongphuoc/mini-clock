#ifndef __SCR_CLOCK_RINGING_H__
#define __SCR_CLOCK_RINGING_H__

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
#include "scr_clock_main.h"
#include "scr_clock_setting_display.h"

#include "mc_clock_ring.h"
#include "mc_clock_time.h"

// Row positions (title, time, hint)
#define RINGING_TITLE_Y (5)
#define RINGING_TIME_Y (30)
#define RINGING_HINT_Y (52)

extern view_dynamic_t dyn_view_scr_clock_ringing;
extern view_screen_t scr_clock_ringing;
extern void scr_clock_ringing_handle(ak_msg_t* msg);

/* Handle AC_DISPLAY_SHOW_RINGING (payload mc_clock_ring_req_t): leave the
 * current screen, cancelling its display timers, and show the ringing screen.
 * Called by task_display before the message is dispatched. */
extern void scr_clock_ringing_enter(ak_msg_t* msg);

#endif //__SCR_CLOCK_RINGING_H__
