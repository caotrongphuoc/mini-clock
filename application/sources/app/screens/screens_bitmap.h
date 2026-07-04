#ifndef __SCREENS_BITMAP_H__
#define __SCREENS_BITMAP_H__

#include "view_render.h"

#define SCR_CLOCK_MENU_ICON_W (24)
#define SCR_CLOCK_MENU_ICON_H (24)
#define SCR_CLOCK_MENU_ICON_BYTES (((SCR_CLOCK_MENU_ICON_W + 7) / 8) * SCR_CLOCK_MENU_ICON_H)

extern const unsigned char PROGMEM bitmap_clock_menu_clock[];
extern const unsigned char PROGMEM bitmap_clock_menu_alarm[];
extern const unsigned char PROGMEM bitmap_clock_menu_stopwatch[];
extern const unsigned char PROGMEM bitmap_clock_menu_timer[];
extern const unsigned char PROGMEM bitmap_clock_menu_setting[];
extern const unsigned char PROGMEM bitmap_clock_menu_exit[];

#endif //__SCREENS_BITMAP_H__
