#ifndef __MC_CLOCK_ALARM_H__
#define __MC_CLOCK_ALARM_H__

#include "ak.h"

#include "app.h"
#include "message.h"
#include "task_list.h"
#include "app_dbg.h"
#include "buzzer.h"
#include "task_mc_rtc.h"

#define MC_CLOCK_ALARM_MAX (5)

typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t enabled;
} mc_clock_alarm_item_t;

typedef struct
{
	uint8_t current_item;
	uint8_t scroll_offset;
	uint8_t total_alarm;
	uint8_t editing;
	uint8_t editing_alarm;
	uint8_t editing_field;
	uint8_t ringing;
	uint8_t alarm_sound;
	mc_clock_alarm_item_t alarm[MC_CLOCK_ALARM_MAX];
} mc_clock_alarm_state_t;

void mc_clock_alarm_handle(ak_msg_t* msg);
void mc_clock_alarm_get_state(mc_clock_alarm_state_t* state);
void mc_clock_alarm_apply_rtc(void);
void mc_clock_alarm_set_sound(uint8_t sound);

#endif //__MC_CLOCK_ALARM_H__
