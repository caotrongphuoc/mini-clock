#ifndef __MC_CLOCK_TIMER_H__
#define __MC_CLOCK_TIMER_H__

#include "ak.h"
#include "app.h"
#include "message.h"
#include "task_list.h"
#include "app_dbg.h"
#include "buzzer.h"

#define MC_CLOCK_TIMER_MAX_MINUTES (1000)
#define MC_CLOCK_TIMER_STEP_MINUTES (1)

typedef enum
{
    MC_TIMER_EDIT_HOUR = 0,
    MC_TIMER_EDIT_MIN,
    MC_TIMER_EDIT_SEC,
} mc_timer_edit_field_t;

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
} mc_clock_timer_state_t;

void mc_clock_timer_handle(ak_msg_t* msg);
void mc_clock_timer_get_state(mc_clock_timer_state_t* state);

#endif //__MC_CLOCK_TIMER_H__
