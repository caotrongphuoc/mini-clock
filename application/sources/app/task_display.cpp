#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_display.h"
#include "scr_clock_ringing.h"

scr_mng_t scr_mng_app;

void task_display(ak_msg_t* msg) {
	/* Alarm, timer and calendar can ring on any screen, so switch here rather than in every
	 * screen handler. */
	if (msg->sig == AC_DISPLAY_SHOW_RINGING) {
		scr_clock_ringing_enter(msg);
	}

	scr_mng_dispatch(msg);
}
