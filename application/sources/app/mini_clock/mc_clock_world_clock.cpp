#include "mc_clock_world_clock.h"

#include "rtc.h"
#include "app_dbg.h"
#include "task_list.h"
#include "mc_clock_alarm.h"

#include <string.h>


static mc_world_clock_state_t world_clock;

/*****************************************************************************/
/* Helper - build UTC label string from offset in minutes                    */
/*****************************************************************************/

static void build_utc_label(char* out, int16_t offset_from_utc_min)
{
    char sign = (offset_from_utc_min >= 0) ? '+' : '-';
    int16_t abs_min = (offset_from_utc_min >= 0)
                        ? offset_from_utc_min
                        : -offset_from_utc_min;
    uint8_t h = (uint8_t)(abs_min / 60);
    uint8_t m = (uint8_t)(abs_min % 60);

    if (m == 0)
    {
        xsprintf(out, "UTC%c%u", sign, h);
    }
    else
    {
        xsprintf(out, "UTC%c%u:%02u", sign, h, m);
    }
}


/*****************************************************************************/
/* Init - World Clock                                                         */
/*****************************************************************************/

/*
 * RTC is set to Vietnam local time (UTC+7).
 * All offsets below are relative to Vietnam time:
 *   offset_minutes = (country_utc_offset_hours * 60) - (7 * 60)
 */

void mc_clock_world_clock_init(void)
{
    /* Vietnam  UTC+7  => offset = 0 */
    strcpy(world_clock.country[0].name, "Vietnam");
    world_clock.country[0].timezone_offset_minutes = 0;
    build_utc_label(world_clock.country[0].utc_label, 7 * 60);

    /* USA (Eastern) UTC-5  => offset = (-5 - 7) * 60 = -720 */
    strcpy(world_clock.country[1].name, "USA");
    world_clock.country[1].timezone_offset_minutes = -720;
    build_utc_label(world_clock.country[1].utc_label, -5 * 60);

    /* England  UTC+0  => offset = (0 - 7) * 60 = -420 */
    strcpy(world_clock.country[2].name, "England");
    world_clock.country[2].timezone_offset_minutes = -420;
    build_utc_label(world_clock.country[2].utc_label, 0);

    /* China  UTC+8  => offset = (8 - 7) * 60 = 60 */
    strcpy(world_clock.country[3].name, "China");
    world_clock.country[3].timezone_offset_minutes = 60;
    build_utc_label(world_clock.country[3].utc_label, 8 * 60);

    /* Japan  UTC+9  => offset = (9 - 7) * 60 = 120 */
    strcpy(world_clock.country[4].name, "Japan");
    world_clock.country[4].timezone_offset_minutes = 120;
    build_utc_label(world_clock.country[4].utc_label, 9 * 60);

    /* France  UTC+1  => offset = (1 - 7) * 60 = -360 */
    strcpy(world_clock.country[5].name, "France");
    world_clock.country[5].timezone_offset_minutes = -360;
    build_utc_label(world_clock.country[5].utc_label, 1 * 60);

    /* Australia (AEDT)  UTC+10  => offset = (10 - 7) * 60 = 180 */
    strcpy(world_clock.country[6].name, "Austria");
    world_clock.country[6].timezone_offset_minutes = 180;
    build_utc_label(world_clock.country[6].utc_label, 10 * 60);

    /* Brazil  UTC-3  => offset = (-3 - 7) * 60 = -600 */
    strcpy(world_clock.country[7].name, "Brazil");
    world_clock.country[7].timezone_offset_minutes = -600;
    build_utc_label(world_clock.country[7].utc_label, -3 * 60);

    /* India  UTC+5:30  => offset = (5*60+30 - 7*60) = -90 */
    strcpy(world_clock.country[8].name, "India");
    world_clock.country[8].timezone_offset_minutes = -90;
    build_utc_label(world_clock.country[8].utc_label, 5 * 60 + 30);

    /* Korea  UTC+9  => offset = (9 - 7) * 60 = 120 */
    strcpy(world_clock.country[9].name, "Korea");
    world_clock.country[9].timezone_offset_minutes = 120;
    build_utc_label(world_clock.country[9].utc_label, 9 * 60);

    world_clock.selected_country = 0;

    world_clock.hour   = 0;
    world_clock.minute = 0;
    world_clock.second = 0;

}

void mc_clock_world_clock_select_country(
        uint8_t index)
{
    if(index < MC_WORLD_CLOCK_MAX_COUNTRY)
    {
        world_clock.selected_country = index;
    }
}

void mc_clock_world_clock_get_state(
        mc_world_clock_state_t* state)
{
    memcpy(state,&world_clock,sizeof(mc_world_clock_state_t));
}

int16_t mc_clock_world_clock_get_selected_offset_minutes(void)
{
    return world_clock.country[world_clock.selected_country].timezone_offset_minutes;
}

static void mc_clock_world_clock_update(void)
{
    rtc_time_t now;
    rtc_get_time(&now);

    /* Convert current RTC time (Vietnam local) to total minutes since midnight */
    int32_t total_minutes = (int32_t)now.hour * 60 + (int32_t)now.min;

    /* Apply offset for selected country */
    int16_t offset = world_clock.country[world_clock.selected_country].timezone_offset_minutes;
    total_minutes += offset;

    /* Wrap around 24 hours (1440 minutes) */
    total_minutes = ((total_minutes % 1440) + 1440) % 1440;

    world_clock.hour   = (uint8_t)(total_minutes / 60);
    world_clock.minute = (uint8_t)(total_minutes % 60);
    world_clock.second = now.sec;
}



void mc_clock_world_clock_handle(
        ak_msg_t* msg)
{
    switch(msg->sig)
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
        }
        break;


        case MC_CLOCK_WORLD_CLOCK_SELECT_UP:
        {
            APP_DBG_SIG("MC_CLOCK_WORLD_CLOCK_SELECT_UP\n");
            if (world_clock.selected_country < MC_WORLD_CLOCK_MAX_COUNTRY - 1)
            {
                world_clock.selected_country++;
            }
            else
            {
                world_clock.selected_country = 0;
            }
            mc_clock_world_clock_update();
            mc_clock_alarm_apply_rtc();
        }
        break;


        case MC_CLOCK_WORLD_CLOCK_SELECT_DOWN:
        {
            APP_DBG_SIG("MC_CLOCK_WORLD_CLOCK_SELECT_DOWN\n");
            if (world_clock.selected_country > 0)
            {
                world_clock.selected_country--;
            }
            else
            {
                world_clock.selected_country = MC_WORLD_CLOCK_MAX_COUNTRY - 1;
            }
            mc_clock_world_clock_update();
            mc_clock_alarm_apply_rtc();
        }
        break;


        default:
        break;

    }

}