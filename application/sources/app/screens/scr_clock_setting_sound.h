#ifndef __SCR_CLOCK_SETTING_SOUND_H__
#define __SCR_CLOCK_SETTING_SOUND_H__

#include "screens.h"
#include "buzzer.h"

#define SCR_CLOCK_SETTING_SOUND_ITEM_NUMBER (4)

typedef enum
{
	SCR_CLOCK_SETTING_SOUND_MUTE,
	SCR_CLOCK_SETTING_SOUND_CHIME,
	SCR_CLOCK_SETTING_SOUND_MUSIC,
	SCR_CLOCK_SETTING_SOUND_BACK,
} scr_clock_setting_sound_item_t;

static const buzzer_sound_t music_list[] = {

    BUZZER_SOUND_ADVENTURE_THEME,
    BUZZER_SOUND_SPACE_TRIP,
    BUZZER_SOUND_BATTLE_READY,
    BUZZER_SOUND_ALARM_CLASSIC,
    BUZZER_SOUND_ALARM_RISING,
    BUZZER_SOUND_ALARM_BEEP_BEEP,
    BUZZER_SOUND_ALARM_MORNING,
    BUZZER_SOUND_ALARM_URGENT,

};

#define MUSIC_LIST_NUMBER (sizeof(music_list) / sizeof(music_list[0]))

void scr_clock_setting_sound_reset(void);

#endif //__SCR_CLOCK_SETTING_SOUND_H__
