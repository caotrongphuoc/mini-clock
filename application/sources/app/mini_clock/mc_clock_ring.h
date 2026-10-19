#ifndef __MC_CLOCK_RING_H__
#define __MC_CLOCK_RING_H__

#include "ak.h"
#include "message.h"

#include "app.h"
#include "task_list.h"

/* Ringing is shown by one screen for every source. The source object plays
 * the first sound and posts AC_DISPLAY_SHOW_RINGING; the ringing screen
 * repeats the sound, times out, and posts the source's dismiss signal. */
#define MC_CLOCK_RING_REPEAT_S  (5)
#define MC_CLOCK_RING_TIMEOUT_S (60)

typedef enum
{
	MC_CLOCK_RING_SOURCE_ALARM = 0,
	MC_CLOCK_RING_SOURCE_TIMER,
	MC_CLOCK_RING_SOURCE_CALENDAR,
	MC_CLOCK_RING_SOURCE_MAX,
} mc_clock_ring_source_t;

/* AC_DISPLAY_SHOW_RINGING payload */
typedef struct
{
	uint8_t source; /* mc_clock_ring_source_t */
	uint8_t sound;  /* buzzer_sound_t */
} mc_clock_ring_req_t;

static inline void mc_clock_ring_request(uint8_t source, uint8_t sound)
{
	mc_clock_ring_req_t req;

	req.source = source;
	req.sound = sound;
	task_post_common_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_RINGING, (uint8_t*)&req, sizeof(req));
}

#endif //__MC_CLOCK_RING_H__
