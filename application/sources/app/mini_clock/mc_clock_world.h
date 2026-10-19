#ifndef __MC_CLOCK_WORLD_CLOCK_H__
#define __MC_CLOCK_WORLD_CLOCK_H__

#include "ak.h"
#include "app.h"
#include "message.h"
#include "rtc.h"
#include "app_dbg.h"
#include "task_list.h"
#include "mc_clock_alarm.h"

#include <string.h>

// Number of cities in the world clock table
#define WORLD_CLOCK_COUNTRY_NUMBER (32)

typedef struct
{
	const char* name;

	/* Offset from Vietnam (UTC+7) in minutes.
	 * e.g. Vietnam=0, USA(UTC-5)=-720, India(UTC+5:30)=-90 */
	int16_t timezone_offset_minutes;

	/* Pre-formatted UTC string, e.g. "UTC+7", "UTC-5", "UTC+5:30" */
	const char* utc_label;

} mc_clock_world_clock_country_t;

typedef struct
{
	uint8_t selected_country;

	uint8_t hour;
	uint8_t minute;
	uint8_t second;

} mc_clock_world_clock_state_t;

void mc_clock_world_clock_init(void);

void mc_clock_world_clock_handle(
    ak_msg_t* msg);

void mc_clock_world_clock_select_country(
    uint8_t index);

void mc_clock_world_clock_get_state(
    mc_clock_world_clock_state_t* state);

/* Read-only country entry from the flash table (index is clamped). */
const mc_clock_world_clock_country_t* mc_clock_world_clock_get_country(uint8_t index);

int16_t mc_clock_world_clock_get_selected_offset_minutes(void);

#endif