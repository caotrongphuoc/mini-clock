#ifndef __SCREENS_H__
#define __SCREENS_H__

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
#include "screens_bitmap.h"

#include "buzzer.h"

#include <math.h>
#include <vector>

// scr_idle
extern view_dynamic_t dyn_view_idle;
extern view_screen_t scr_idle;
extern void scr_idle_handle(ak_msg_t* msg);

// scr_startup
extern view_dynamic_t dyn_view_startup;
extern view_screen_t scr_startup;
extern void scr_startup_handle(ak_msg_t* msg);

// scr_clock_main
extern view_dynamic_t dyn_view_scr_clock_main;
extern view_screen_t scr_clock_main;
extern void scr_clock_main_handle(ak_msg_t* msg);

// scr_clock_menu
extern view_dynamic_t dyn_view_scr_clock_menu;
extern view_screen_t scr_clock_menu;
extern void scr_clock_menu_handle(ak_msg_t* msg);

// scr_clock_setting
extern view_dynamic_t dyn_view_scr_clock_setting;
extern view_screen_t scr_clock_setting;
extern void scr_clock_setting_handle(ak_msg_t* msg);

// scr_clock_time_setting
extern view_dynamic_t dyn_view_scr_clock_time_setting;
extern view_screen_t scr_clock_time_setting;
extern void scr_clock_time_setting_handle(ak_msg_t* msg);

#endif //__SCREENS_H__
