#ifndef __SCR_CLOCK_SETTING_SOUND_H__
#define __SCR_CLOCK_SETTING_SOUND_H__

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

#include "buzzer.h"
#include "app_eeprom.h"

#include "screens.h"
#include "scr_clock_setting.h"

#include "mc_clock_time.h"
#include "mc_clock_alarm.h"

// Total number of menu items
#define SETTING_SOUND_ITEM_NUMBER (4)

typedef enum
{
	SETTING_SOUND_MUTE,
	SETTING_SOUND_CHIME,
	SETTING_SOUND_ALARM,
	SETTING_SOUND_BACK,
} scr_clock_setting_sound_item_t;

// Number of selectable alarm melodies
#define ALARM_SOUND_LIST_NUMBER (8)

extern view_dynamic_t dyn_view_scr_clock_setting_sound;
extern view_screen_t scr_clock_setting_sound;
extern void scr_clock_setting_sound_handle(ak_msg_t* msg);

extern void scr_clock_setting_sound_reset(void);
extern void scr_clock_setting_sound_apply(void);

#endif //__SCR_CLOCK_SETTING_SOUND_H__
