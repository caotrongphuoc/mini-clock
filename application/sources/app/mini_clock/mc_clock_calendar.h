#ifndef __MC_CLOCK_CALENDAR_H__
#define __MC_CLOCK_CALENDAR_H__

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
#include "rtc.h"
#include "eeprom.h"
#include "app_eeprom.h"

#include "mc_clock_time.h"
#include "mc_clock_world.h"
#include "mc_clock_alarm.h"
#include "mc_clock_ring.h"
#include "mc_clock_date.h"

// Number of events in the calendar (and in the EEPROM bank)
#define CALENDAR_EVENT_NUMBER (10)

// Events shown at once in the day list
#define CALENDAR_LIST_VISIBLE (2)

// Event index meaning "no event selected"
#define CALENDAR_EVENT_NONE (255)

// Start and end hour of a newly added event
#define CALENDAR_NEW_START_HOUR (8)
#define CALENDAR_NEW_END_HOUR (9)

// Event category (stored in the EEPROM bank)
typedef enum
{
	CALENDAR_CATEGORY_WORK = 0,
	CALENDAR_CATEGORY_MEETING,
	CALENDAR_CATEGORY_PERSONAL,
	CALENDAR_CATEGORY_REMINDER,
	CALENDAR_CATEGORY_MAX,
} mc_clock_calendar_category_t;

// Screen mode
typedef enum
{
	CALENDAR_MODE_MONTH = 0, // month grid
	CALENDAR_MODE_LIST,      // events of the selected day
	CALENDAR_MODE_EDIT,      // add or edit an event
} mc_clock_calendar_mode_t;

// Editor fields, in the order NEXT_FIELD walks them
typedef enum
{
	CALENDAR_FIELD_YEAR = 0,
	CALENDAR_FIELD_MONTH,
	CALENDAR_FIELD_DAY,
	CALENDAR_FIELD_CATEGORY,
	CALENDAR_FIELD_START_HOUR,
	CALENDAR_FIELD_START_MIN,
	CALENDAR_FIELD_END_HOUR,
	CALENDAR_FIELD_END_MIN,
	CALENDAR_FIELD_ALARM,
	CALENDAR_FIELD_MAX,
} mc_clock_calendar_field_t;

// Repeat mode (stored in the EEPROM bank)
typedef enum
{
	CALENDAR_REPEAT_NONE = 0,
	CALENDAR_REPEAT_DAILY,
	CALENDAR_REPEAT_WEEKLY,
	CALENDAR_REPEAT_MONTHLY,
	CALENDAR_REPEAT_YEARLY,
	CALENDAR_REPEAT_MAX,
} mc_clock_calendar_repeat_t;

typedef struct
{
	uint16_t year;
	uint8_t month; // 1-12
	uint8_t day;   // 1-31

	uint8_t start_hour; // 0-23
	uint8_t start_min;  // 0-59
	uint8_t end_hour;   // 0-23
	uint8_t end_min;    // 0-59

	uint8_t category; // mc_clock_calendar_category_t

	uint8_t alarm_enabled;
	uint8_t alarm_sound; // buzzer sound ID
	uint8_t repeat;      // mc_clock_calendar_repeat_t
	uint8_t alarm_fired; // 1 = reminder already rang this occurrence
} mc_clock_calendar_event_t;

typedef struct
{
	// Today (selected city)
	uint16_t today_year;
	uint8_t today_month;
	uint8_t today_day;

	// Month view
	uint16_t view_year;
	uint8_t view_month;
	uint8_t selected_day;

	// Day list
	uint8_t total_events;
	uint8_t current_event; // index in events[]
	uint8_t scroll_offset;

	// Editor
	uint8_t editing_event; // index in events[]
	uint8_t editing_field; // mc_clock_calendar_field_t
	uint8_t is_new_event;  // 1 when adding a new event

	// Reminder
	uint8_t ringing;
	uint8_t ringing_event;

	uint8_t mode; // mc_clock_calendar_mode_t

	mc_clock_calendar_event_t events[CALENDAR_EVENT_NUMBER];
} mc_clock_calendar_t;

// Bank stored in EEPROM (count and events only)
typedef struct
{
	uint8_t total_events;
	mc_clock_calendar_event_t events[CALENDAR_EVENT_NUMBER];
} mc_clock_calendar_bank_t;

extern mc_clock_calendar_t mc_clock_calendar;

extern const char* const mc_clock_calendar_category_name[CALENDAR_CATEGORY_MAX];

extern void mc_clock_calendar_init(void);
extern void mc_clock_calendar_load_from_eeprom(void);
extern void mc_clock_calendar_sync(void);
extern uint8_t mc_clock_calendar_has_event_on_day(uint16_t year, uint8_t month, uint8_t day);

#endif //__MC_CLOCK_CALENDAR_H__
