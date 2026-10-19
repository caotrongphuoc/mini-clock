#ifndef __MC_CLOCK_TIMER_H__
#define __MC_CLOCK_TIMER_H__

#include "ak.h"
#include "app.h"
#include "message.h"
#include "task_list.h"
#include "app_dbg.h"
#include "buzzer.h"

// Largest value of the hour field
#define TIMER_HOUR_MAX (23)

typedef enum
{
	TIMER_EDIT_HOUR = 0,
	TIMER_EDIT_MIN,
	TIMER_EDIT_SEC,
} mc_clock_timer_edit_field_t;

typedef struct
{
	uint8_t set_hour;
	uint8_t set_min;
	uint8_t set_sec;

	uint8_t remaining_hour;
	uint8_t remaining_min;
	uint8_t remaining_sec;

	uint8_t running;
	uint8_t finished;

	uint8_t edit_field;
} mc_clock_timer_t;

void mc_clock_timer_handle(ak_msg_t* msg);
extern mc_clock_timer_t mc_clock_timer;

#endif //__MC_CLOCK_TIMER_H__
