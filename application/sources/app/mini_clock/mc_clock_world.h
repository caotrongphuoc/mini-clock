#ifndef __MC_CLOCK_WORLD_H__
#define __MC_CLOCK_WORLD_H__

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

#include "rtc.h"

// Number of cities in the world clock table
#define WORLD_CLOCK_COUNTRY_NUMBER (32)

typedef struct
{
	const char* name;
	int16_t timezone_offset_minutes; // offset from Vietnam (UTC+7), e.g. USA (UTC-5) = -720
	const char* utc_label;           // e.g. "UTC+7", "UTC+5:30"
} mc_clock_world_clock_country_t;

typedef struct
{
	uint8_t selected_country;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} mc_clock_world_clock_t;

extern mc_clock_world_clock_t mc_clock_world_clock;

extern void mc_clock_world_clock_init(void);
extern void mc_clock_world_clock_select_country(uint8_t index);
extern const mc_clock_world_clock_country_t* mc_clock_world_clock_get_country(uint8_t index);
extern int16_t mc_clock_world_clock_get_selected_offset_minutes(void);

#endif //__MC_CLOCK_WORLD_H__
