#ifndef __SCR_CLOCK_ALARM_H__
#define __SCR_CLOCK_ALARM_H__

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

#include "mc_clock_alarm.h"

extern view_dynamic_t dyn_view_scr_clock_alarm;
extern view_screen_t scr_clock_alarm;
extern void scr_clock_alarm_handle(ak_msg_t* msg);

#endif //__SCR_CLOCK_ALARM_H__
