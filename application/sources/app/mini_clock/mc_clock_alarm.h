#ifndef __MC_CLOCK_ALARM_H__
#define __MC_CLOCK_ALARM_H__

#include <stdio.h>
#include <string.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"
#include "eeprom.h"
#include "app_eeprom.h"

#include "task_mc_rtc.h"
#include "mc_clock_world.h"
#include "mc_clock_ring.h"

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

// Editor fields, in the order MODE walks them
typedef enum
{
	ALARM_EDIT_HOUR = 0,
	ALARM_EDIT_MINUTE,
	ALARM_EDIT_ENABLE,
	ALARM_EDIT_DELETE,
	ALARM_EDIT_SAVE,
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

	// Ringing state and selected melody
	uint8_t ringing;
	buzzer_sound_t sound;

	// Alarm list
	mc_clock_alarm_item_t alarm[ALARM_NUMBER];
} mc_clock_alarm_t;

// Bank stored in EEPROM (count and list only)
typedef struct
{
	uint8_t total_alarm;
	mc_clock_alarm_item_t alarm[ALARM_NUMBER];
} mc_clock_alarm_bank_t;

extern mc_clock_alarm_t mc_clock_alarm;

extern void mc_clock_alarm_load_from_eeprom(void);
extern void mc_clock_alarm_set_sound(uint8_t sound);
extern uint8_t mc_clock_alarm_get_sound(void);

#endif //__MC_CLOCK_ALARM_H__
