#ifndef __MC_CLOCK_RING_H__
#define __MC_CLOCK_RING_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

// Seconds between melody repeats while ringing
#define RING_REPEAT_S (5)

// Seconds before ringing stops by itself
#define RING_TIMEOUT_S (60)

// Object that asks to ring
typedef enum
{
	RING_SOURCE_ALARM = 0,
	RING_SOURCE_TIMER,
	RING_SOURCE_CALENDAR,
	RING_SOURCE_MAX,
} mc_clock_ring_source_t;

// AC_DISPLAY_SHOW_RINGING payload
typedef struct
{
	uint8_t source; // mc_clock_ring_source_t
	uint8_t sound;  // buzzer_sound_t
} mc_clock_ring_req_t;

/* Ringing is shown by one screen for every source. The source object plays
 * the first sound and calls this; the ringing screen repeats the sound, times
 * out, and posts the source's dismiss signal. */
static inline void mc_clock_ring_request(uint8_t source, uint8_t sound)
{
	mc_clock_ring_req_t req;

	req.source = source;
	req.sound = sound;
	task_post_common_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_RINGING, (uint8_t*)&req, sizeof(req));
}

#endif //__MC_CLOCK_RING_H__
