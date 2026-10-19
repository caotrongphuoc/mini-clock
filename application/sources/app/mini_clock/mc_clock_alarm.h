#ifndef __MC_CLOCK_ALARM_H__
#define __MC_CLOCK_ALARM_H__

#include "ak.h"

#include "app.h"
#include "message.h"
#include "task_list.h"
#include "app_dbg.h"
#include "buzzer.h"
#include "task_mc_rtc.h"

// Number of alarms in the list (and in the EEPROM bank)
#define ALARM_NUMBER (10)

// Rows shown at once in the alarm list
#define ALARM_LIST_VISIBLE (3)

// Time of a newly added alarm
#define ALARM_NEW_HOUR (8)
#define ALARM_NEW_MINUTE (0)

typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t enabled;

} mc_clock_alarm_item_t;

typedef enum
{
	ALARM_EDIT_HOUR = 0,
	ALARM_EDIT_MINUTE = 1,
	ALARM_EDIT_ENABLE = 2,
	ALARM_EDIT_DELETE = 3,
	ALARM_EDIT_SAVE = 4,

} mc_clock_alarm_edit_field_t;

typedef struct
{
	// List navigation
	uint8_t current_item;
	uint8_t scroll_offset;
	uint8_t total_alarm;

	// Editing state
	uint8_t editing;
	uint8_t editing_alarm;

	mc_clock_alarm_edit_field_t editing_field;

	// Alarm ringing state
	uint8_t ringing;

	// Selected alarm sound
	buzzer_sound_t sound;

	// Alarm storage
	mc_clock_alarm_item_t alarm[ALARM_NUMBER];

} mc_clock_alarm_state_t;

void mc_clock_alarm_handle(ak_msg_t* msg);

void mc_clock_alarm_get_state(
    mc_clock_alarm_state_t* state);

void mc_clock_alarm_set_sound(uint8_t sound);
uint8_t mc_clock_alarm_get_sound(void);

/* Persistent bank stored in EEPROM (total + alarm array only) */
typedef struct
{
	uint8_t total_alarm;
	mc_clock_alarm_item_t alarm[ALARM_NUMBER];
} mc_clock_alarm_bank_t;

void mc_clock_alarm_load_from_eeprom(void);

#endif // __MC_CLOCK_ALARM_H__