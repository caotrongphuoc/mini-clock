#ifndef __MC_COMPASS_H__
#define __MC_COMPASS_H__

#include "ak.h"

enum
{
	MC_CLOCK_COMPASS_SETUP,
	MC_CLOCK_COMPASS_UPDATE,
};

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;

	float heading;

} mc_clock_compass_state_t;

void mc_clock_compass_get_state(
    mc_clock_compass_state_t* state);

void mc_clock_compass_handle(
    ak_msg_t* msg);

#endif