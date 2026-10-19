#ifndef __MC_CLOCK_CALENDAR_H__
#define __MC_CLOCK_CALENDAR_H__

#include "ak.h"
#include "app.h"
#include "message.h"
#include "rtc.h"
#include "buzzer.h"
#include "mc_clock_time.h"
#include "mc_clock_world.h"
#include "app_dbg.h"
#include "task_list.h"

#include <string.h>

// Number of events in the calendar (and in the EEPROM bank)
#define CALENDAR_EVENT_NUMBER (10)

// Events shown at once in the day list
#define CALENDAR_LIST_VISIBLE (2)

// Event index meaning "no event selected"
#define CALENDAR_EVENT_NONE (255)

// Start and end hour of a newly added event
#define CALENDAR_NEW_START_HOUR (8)
#define CALENDAR_NEW_END_HOUR (9)

/*****************************************************************************/
/* Category                                                                   */
/*****************************************************************************/

typedef enum
{
	CALENDAR_CATEGORY_WORK = 0,
	CALENDAR_CATEGORY_MEETING = 1,
	CALENDAR_CATEGORY_PERSONAL = 2,
	CALENDAR_CATEGORY_REMINDER = 3,
	CALENDAR_CATEGORY_MAX
} mc_clock_calendar_category_t;

extern const char* const mc_clock_calendar_category_name[CALENDAR_CATEGORY_MAX];

/*****************************************************************************/
/* Event struct                                                               */
/*****************************************************************************/

typedef struct
{
	uint16_t year;
	uint8_t month; /* 1-12 */
	uint8_t day;   /* 1-31 */

	uint8_t start_hour; /* 0-23 */
	uint8_t start_min;  /* 0-59 */

	uint8_t end_hour; /* 0-23 */
	uint8_t end_min;  /* 0-59 */

	uint8_t category; /* mc_clock_calendar_category_t */

	uint8_t alarm_enabled;
	uint8_t alarm_sound; /* buzzer sound ID */

	uint8_t repeat;      /* repeat mode */
	uint8_t alarm_fired; /* 1 = alarm already fired this occurrence */

} mc_clock_calendar_event_t;

/*****************************************************************************/
/* Calendar screen mode                                                       */
/*****************************************************************************/

typedef enum
{
	CALENDAR_MODE_MONTH = 0, /* Monthly grid view            */
	CALENDAR_MODE_LIST = 1,  /* Event list for selected day  */
	CALENDAR_MODE_EDIT = 2,  /* Edit / Add event             */
} mc_clock_calendar_mode_t;

/*****************************************************************************/
/* State struct                                                               */
/*****************************************************************************/

typedef struct
{
	/* Today from RTC */
	uint16_t today_year;
	uint8_t today_month;
	uint8_t today_day;

	/* Calendar view state */
	uint16_t view_year;
	uint8_t view_month;
	uint8_t selected_day; /* day highlighted in grid     */

	/* Event list state */
	uint8_t total_events;
	uint8_t current_event; /* index in events[]           */
	uint8_t scroll_offset;

	/* Edit state */
	uint8_t editing_event; /* index being edited          */
	uint8_t editing_field; /* 0-based field cursor        */
	uint8_t is_new_event;  /* 1 when adding a new event   */

	/* Alarm state */
	uint8_t ringing;
	uint8_t ringing_event;

	/* Screen mode */
	uint8_t mode; /* mc_clock_calendar_mode_t          */

	mc_clock_calendar_event_t events[CALENDAR_EVENT_NUMBER];

} mc_clock_calendar_t;

/*****************************************************************************/
/* Edit fields (used by editing_field)                                        */
/*****************************************************************************/

typedef enum
{
	CALENDAR_FIELD_YEAR = 0,
	CALENDAR_FIELD_MONTH = 1,
	CALENDAR_FIELD_DAY = 2,
	CALENDAR_FIELD_CATEGORY = 3,
	CALENDAR_FIELD_START_HOUR = 4,
	CALENDAR_FIELD_START_MIN = 5,
	CALENDAR_FIELD_END_HOUR = 6,
	CALENDAR_FIELD_END_MIN = 7,
	CALENDAR_FIELD_ALARM = 8,
	CALENDAR_FIELD_MAX
} mc_clock_calendar_field_t;

/*****************************************************************************/
/* Repeat mode                                                               */
/*****************************************************************************/

typedef enum
{
	CALENDAR_REPEAT_NONE = 0,
	CALENDAR_REPEAT_DAILY,
	CALENDAR_REPEAT_WEEKLY,
	CALENDAR_REPEAT_MONTHLY,
	CALENDAR_REPEAT_YEARLY,
	CALENDAR_REPEAT_MAX

} mc_clock_calendar_repeat_t;

/*****************************************************************************/
/* Public API                                                                 */
/*****************************************************************************/

void mc_clock_calendar_init(void);

void mc_clock_calendar_handle(ak_msg_t* msg);

extern mc_clock_calendar_t mc_clock_calendar;

uint8_t mc_clock_calendar_has_event_on_day(uint16_t year, uint8_t month, uint8_t day);

void mc_clock_calendar_sync(void);

/* Persistent bank stored in EEPROM (total + events array) */
typedef struct
{
	uint8_t total_events;
	mc_clock_calendar_event_t events[CALENDAR_EVENT_NUMBER];
} mc_clock_calendar_bank_t;

void mc_clock_calendar_load_from_eeprom(void);

#endif /* __MC_CLOCK_CALENDAR_H__ */