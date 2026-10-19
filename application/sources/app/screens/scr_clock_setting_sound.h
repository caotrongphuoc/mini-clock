#ifndef __SCR_CLOCK_SETTING_SOUND_H__
#define __SCR_CLOCK_SETTING_SOUND_H__

#include "screens.h"
#include "buzzer.h"

// Total number of menu items
#define SETTING_SOUND_ITEM_NUMBER (4)

typedef enum
{
	SETTING_SOUND_MUTE,
	SETTING_SOUND_CHIME,
	SETTING_SOUND_ALARM,
	SETTING_SOUND_BACK,
} scr_clock_setting_sound_item_t;

static const buzzer_sound_t alarm_sound_list[] = {

    BUZZER_SOUND_ADVENTURE_THEME,
    BUZZER_SOUND_SPACE_TRIP,
    BUZZER_SOUND_BATTLE_READY,
    BUZZER_SOUND_ALARM_CLASSIC,
    BUZZER_SOUND_ALARM_RISING,
    BUZZER_SOUND_ALARM_BEEP_BEEP,
    BUZZER_SOUND_ALARM_MORNING,
    BUZZER_SOUND_ALARM_URGENT,

};

// Number of selectable alarm melodies
#define ALARM_SOUND_LIST_NUMBER (sizeof(alarm_sound_list) / sizeof(alarm_sound_list[0]))

void scr_clock_setting_sound_reset(void);
void scr_clock_setting_sound_apply(void);

#endif //__SCR_CLOCK_SETTING_SOUND_H__
