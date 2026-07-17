#ifndef __SCR_CLOCK_COMPASS_H__
#define __SCR_CLOCK_COMPASS_H__

#include "ak.h"
#include "mc_compass.h"
#include "task_list.h"
#include "timer.h"
#include "scr_clock_menu.h"

#include <math.h>

extern view_screen_t scr_clock_compass;

void scr_clock_compass_handle(
    ak_msg_t* msg);

#endif