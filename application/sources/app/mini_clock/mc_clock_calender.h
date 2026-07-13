#ifndef __MC_CLOCK_CALENDAR_H__
#define __MC_CLOCK_CALENDAR_H__

#include "ak.h"
#include "app.h"
#include "message.h"

/*****************************************************************************/
/* Constants                                                                  */
/*****************************************************************************/

#define MC_CALENDAR_MAX_EVENTS  (10)


/*****************************************************************************/
/* Category                                                                   */
/*****************************************************************************/

typedef enum
{
    MC_CAL_CAT_WORK     = 0,
    MC_CAL_CAT_MEETING  = 1,
    MC_CAL_CAT_PERSONAL = 2,
    MC_CAL_CAT_REMINDER = 3,
    MC_CAL_CAT_MAX
} mc_calendar_category_t;

extern const char* const MC_CAL_CAT_NAME[MC_CAL_CAT_MAX];


/*****************************************************************************/
/* Event struct                                                               */
/*****************************************************************************/

typedef struct
{
    uint16_t year;
    uint8_t  month;       /* 1-12 */
    uint8_t  day;         /* 1-31 */
    uint8_t  start_hour;  /* 0-23 */
    uint8_t  start_min;   /* 0-59 */
    uint8_t  end_hour;    /* 0-23 */
    uint8_t  end_min;     /* 0-59 */
    uint8_t  category;    /* mc_calendar_category_t */
    uint8_t  alarm_enabled;
    uint8_t  alarm_fired; /* 1 = alarm already fired this occurrence */
} mc_calendar_event_t;


/*****************************************************************************/
/* Calendar screen mode                                                       */
/*****************************************************************************/

typedef enum
{
    MC_CAL_MODE_MONTH = 0,  /* Monthly grid view            */
    MC_CAL_MODE_LIST  = 1,  /* Event list for selected day  */
    MC_CAL_MODE_EDIT  = 2,  /* Edit / Add event             */
} mc_calendar_mode_t;


/*****************************************************************************/
/* State struct                                                               */
/*****************************************************************************/

typedef struct
{
    /* Today from RTC */
    uint16_t today_year;
    uint8_t  today_month;
    uint8_t  today_day;

    /* Calendar view state */
    uint16_t view_year;
    uint8_t  view_month;
    uint8_t  selected_day;   /* day highlighted in grid     */

    /* Event list state */
    uint8_t  total_events;
    uint8_t  current_event;  /* index in events[]           */
    uint8_t  scroll_offset;

    /* Edit state */
    uint8_t  editing_event;  /* index being edited          */
    uint8_t  editing_field;  /* 0-based field cursor        */
    uint8_t  is_new_event;   /* 1 when adding a new event   */

    /* Alarm state */
    uint8_t  ringing;
    uint8_t  ringing_event;

    /* Screen mode */
    uint8_t  mode;           /* mc_calendar_mode_t          */

    mc_calendar_event_t events[MC_CALENDAR_MAX_EVENTS];

} mc_calendar_state_t;


/*****************************************************************************/
/* Edit fields (used by editing_field)                                        */
/*****************************************************************************/

typedef enum
{
    MC_CAL_FIELD_YEAR       = 0,
    MC_CAL_FIELD_MONTH      = 1,
    MC_CAL_FIELD_DAY        = 2,
    MC_CAL_FIELD_CATEGORY   = 3,
    MC_CAL_FIELD_START_HOUR = 4,
    MC_CAL_FIELD_START_MIN  = 5,
    MC_CAL_FIELD_END_HOUR   = 6,
    MC_CAL_FIELD_END_MIN    = 7,
    MC_CAL_FIELD_ALARM      = 8,
    MC_CAL_FIELD_MAX
} mc_calendar_field_t;


/*****************************************************************************/
/* Public API                                                                 */
/*****************************************************************************/

void mc_clock_calendar_init(void);

void mc_clock_calendar_handle(ak_msg_t* msg);

void mc_clock_calendar_get_state(mc_calendar_state_t* state);

uint8_t mc_calendar_days_in_month(uint8_t month, uint16_t year);

uint8_t mc_calendar_has_event_on_day(uint16_t year, uint8_t month, uint8_t day);

void mc_clock_calendar_sync(void);

#endif /* __MC_CLOCK_CALENDAR_H__ */