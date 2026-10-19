#ifndef __MC_CLOCK_TIMER_H__
#define __MC_CLOCK_TIMER_H__

#include <stdio.h>

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "buzzer.h"

#include "mc_clock_alarm.h"
#include "mc_clock_ring.h"

// Largest value of the hour field
#define TIMER_HOUR_MAX (23)

// Editor fields, in the order MODE+UP walks them
typedef enum
{
	TIMER_EDIT_HOUR = 0,
	TIMER_EDIT_MIN,
	TIMER_EDIT_SEC,
} mc_clock_timer_edit_field_t;

typedef struct
{
	// Value set by the user
	uint8_t set_hour;
	uint8_t set_min;
	uint8_t set_sec;

	// Countdown
	uint8_t remaining_hour;
	uint8_t remaining_min;
	uint8_t remaining_sec;

	uint8_t running;
	uint8_t finished;
	uint8_t edit_field; // mc_clock_timer_edit_field_t
} mc_clock_timer_t;

extern mc_clock_timer_t mc_clock_timer;

#endif //__MC_CLOCK_TIMER_H__
