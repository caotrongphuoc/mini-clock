#ifndef __SCR_CLOCK_ANALOG_H__
#define __SCR_CLOCK_ANALOG_H__

#include <math.h>

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

#include "mc_clock_time.h"
#include "mc_clock_world.h"

extern view_dynamic_t dyn_view_scr_clock_analog;
extern view_screen_t scr_clock_analog;
extern void scr_clock_analog_handle(ak_msg_t* msg);

#endif //__SCR_CLOCK_ANALOG_H__
