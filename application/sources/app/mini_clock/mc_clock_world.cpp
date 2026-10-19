#include "mc_clock_world.h"

/* Variable Declaration - World clock object */
mc_clock_world_clock_t mc_clock_world_clock;

/*
 * RTC is set to Vietnam local time (UTC+7).
 * All offsets below are relative to Vietnam time:
 *   offset_minutes = (country_utc_offset_hours * 60) - (7 * 60)
 * The table lives in flash; only the selection and current time are in RAM.
 */
static const mc_clock_world_clock_country_t world_clock_country[WORLD_CLOCK_COUNTRY_NUMBER] = {
    {"Vietnam", 0, "UTC+7"},
    {"England", -420, "UTC+0"},
    {"Portugal", -420, "UTC+0"},
    {"France", -360, "UTC+1"},
    {"Germany", -360, "UTC+1"},
    {"Egypt", -300, "UTC+2"},
    {"Greece", -300, "UTC+2"},
    {"Moscow", -240, "UTC+3"},
    {"Dubai", -180, "UTC+4"},
    {"Iran", -210, "UTC+3:30"},
    {"Pakistan", -120, "UTC+5"},
    {"India", -90, "UTC+5:30"},
    {"Dhaka", -60, "UTC+6"},
    {"Myanmar", -30, "UTC+6:30"},
    {"Thailand", 0, "UTC+7"},
    {"Jakarta", 0, "UTC+7"},
    {"Singapore", 60, "UTC+8"},
    {"China", 60, "UTC+8"},
    {"Korea", 120, "UTC+9"},
    {"Japan", 120, "UTC+9"},
    {"Adelaide", 150, "UTC+9:30"},
    {"Sydney", 180, "UTC+10"},
    {"Auckland", 300, "UTC+12"},
    {"Azores", -480, "UTC-1"},
    {"Brazil", -600, "UTC-3"},
    {"Caracas", -660, "UTC-4"},
    {"USA", -720, "UTC-5"},
    {"Chicago", -780, "UTC-6"},
    {"Denver", -840, "UTC-7"},
    {"LA", -900, "UTC-8"},
    {"Alaska", -960, "UTC-9"},
    {"Hawaii", -1020, "UTC-10"},
};

/* Init - World Clock                                                         */
void mc_clock_world_clock_init(void)
{
	mc_clock_world_clock.selected_country = 0;

	mc_clock_world_clock.hour = 0;
	mc_clock_world_clock.minute = 0;
	mc_clock_world_clock.second = 0;
}

void mc_clock_world_clock_select_country(
    uint8_t index)
{
	if (index < WORLD_CLOCK_COUNTRY_NUMBER)
	{
		mc_clock_world_clock.selected_country = index;
	}
}

const mc_clock_world_clock_country_t* mc_clock_world_clock_get_country(uint8_t index)
{
	if (index >= WORLD_CLOCK_COUNTRY_NUMBER)
	{
		index = 0;
	}

	return &world_clock_country[index];
}

int16_t mc_clock_world_clock_get_selected_offset_minutes(void)
{
	return world_clock_country[mc_clock_world_clock.selected_country].timezone_offset_minutes;
}

static void mc_clock_world_clock_update(void)
{
	rtc_time_t now;
	rtc_get_time(&now);

	/* Convert current RTC time (Vietnam local) to total minutes since midnight */
	int32_t total_minutes = (int32_t)now.hour * 60 + (int32_t)now.min;

	/* Apply offset for selected country */
	int16_t offset = world_clock_country[mc_clock_world_clock.selected_country].timezone_offset_minutes;
	total_minutes += offset;

	/* Wrap around 24 hours (1440 minutes) */
	total_minutes = ((total_minutes % 1440) + 1440) % 1440;

	mc_clock_world_clock.hour = (uint8_t)(total_minutes / 60);
	mc_clock_world_clock.minute = (uint8_t)(total_minutes % 60);
	mc_clock_world_clock.second = now.sec;
}

void mc_clock_world_clock_handle(
    ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_WORLD_CLOCK_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_WORLD_CLOCK_SETUP\n");
		mc_clock_world_clock_update();
	}
	break;

	case MC_CLOCK_WORLD_CLOCK_UPDATE:
	{
		APP_DBG_SIG("MC_CLOCK_WORLD_CLOCK_UPDATE\n");
		mc_clock_world_clock_update();

		/* Redraw with the new value; the tick that requested it was rendered
		 * before this update ran, which made the clock show the previous second. */
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_RENDER_SCREEN);
	}
	break;

	case MC_CLOCK_WORLD_CLOCK_SELECT_UP:
	{
		APP_DBG_SIG("MC_CLOCK_WORLD_CLOCK_SELECT_UP\n");
		if (mc_clock_world_clock.selected_country < WORLD_CLOCK_COUNTRY_NUMBER - 1)
		{
			mc_clock_world_clock.selected_country++;
		}
		else
		{
			mc_clock_world_clock.selected_country = 0;
		}
		clock_setting_data.world_country = mc_clock_world_clock.selected_country;
		mc_clock_setting_write(&clock_setting_data);
		mc_clock_world_clock_update();
		/* Alarm times are stored in the selected country's local time;
		 * reprogram RTC so the alarm fires at the correct raw time. */
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_REARM);
	}
	break;

	case MC_CLOCK_WORLD_CLOCK_SELECT_DOWN:
	{
		APP_DBG_SIG("MC_CLOCK_WORLD_CLOCK_SELECT_DOWN\n");
		if (mc_clock_world_clock.selected_country > 0)
		{
			mc_clock_world_clock.selected_country--;
		}
		else
		{
			mc_clock_world_clock.selected_country = WORLD_CLOCK_COUNTRY_NUMBER - 1;
		}
		clock_setting_data.world_country = mc_clock_world_clock.selected_country;
		mc_clock_setting_write(&clock_setting_data);
		mc_clock_world_clock_update();
		/* Alarm times are stored in the selected country's local time;
		 * reprogram RTC so the alarm fires at the correct raw time. */
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_REARM);
	}
	break;

	default:
		break;
	}
}