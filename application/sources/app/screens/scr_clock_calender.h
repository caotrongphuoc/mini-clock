#ifndef __SCR_CALENDAR_H__
#define __SCR_CALENDAR_H__


#include "screens.h"
#include "mc_clock_calender.h"
#include "scr_clock_menu.h"


/* View object (registered in screens.h) */
extern view_dynamic_t dyn_view_scr_clock_calender;
extern view_screen_t  scr_clock_calender;


void scr_calendar_show(void);

void scr_clock_calender_handle(ak_msg_t* msg);



#endif /* __SCR_CALENDAR_H__ */