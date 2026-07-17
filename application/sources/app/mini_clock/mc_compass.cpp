#include "mc_compass.h"
#include "qmc5885l.h"
#include <string.h>

static mc_clock_compass_state_t compass_state;

static qmc5885l_raw_t compass_raw;

void mc_clock_compass_get_state(
    mc_clock_compass_state_t* state)
{
	memcpy(state,
	       &compass_state,
	       sizeof(compass_state));
}

void mc_clock_compass_handle(
    ak_msg_t* msg)
{
	switch (msg->sig)
	{

	case MC_CLOCK_COMPASS_SETUP:
	{
		qmc5885l_init();

		memset(&compass_raw, 0, sizeof(compass_raw));

		compass_state.heading = 0;
		compass_state.x = 0;
		compass_state.y = 0;
		compass_state.z = 0;
	}
	break;

	case MC_CLOCK_COMPASS_UPDATE:
	{
		if (qmc5885l_read(&compass_raw))
		{
			compass_state.x = compass_raw.x;
			compass_state.y = compass_raw.y;
			compass_state.z = compass_raw.z;

			compass_state.heading =
			    qmc5885l_get_heading(&compass_raw);
		}
	}
	break;

	default:
		break;
	}
}