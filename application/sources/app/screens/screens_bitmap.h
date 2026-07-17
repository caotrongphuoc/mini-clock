#ifndef __SCREENS_BITMAP_H__
#define __SCREENS_BITMAP_H__

#include "view_render.h"

#define SCR_CLOCK_MENU_ICON_W (22)
#define SCR_CLOCK_MENU_ICON_H (22)
#define SCR_CLOCK_MENU_ICON_BYTES (((SCR_CLOCK_MENU_ICON_W + 7) / 8) * SCR_CLOCK_MENU_ICON_H)

extern const unsigned char PROGMEM bitmap_clock_menu_clock[];
extern const unsigned char PROGMEM bitmap_clock_menu_alarm[];
extern const unsigned char PROGMEM bitmap_clock_menu_stopwatch[];
extern const unsigned char PROGMEM bitmap_clock_menu_timer[];
extern const unsigned char PROGMEM bitmap_clock_menu_setting[];
extern const unsigned char PROGMEM bitmap_calendar_display[];
extern const unsigned char PROGMEM bitmap_world_clock[];
extern const unsigned char PROGMEM bitmap_compass[];    
extern const unsigned char PROGMEM bitmap_clock_menu_exit[];

extern const unsigned char PROGMEM bitmap_clock_setting_speaker_on[];
extern const unsigned char PROGMEM bitmap_clock_setting_speaker_off[];

extern const unsigned char PROGMEM bitmap_bell_off[];
extern const unsigned char PROGMEM bitmap_bell_ring[];
extern const unsigned char PROGMEM bitmap_clock_4[];
extern const unsigned char PROGMEM bitmap_log_out[];
extern const unsigned char PROGMEM bitmap_monitor[];
extern const unsigned char PROGMEM bitmap_rotate_ccw[];
extern const unsigned char PROGMEM bitmap_volume_2[];

#endif //__SCREENS_BITMAP_H__
