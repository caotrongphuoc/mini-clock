#include "mc_clock_world_clock.h"

static mc_world_clock_state_t world_clock;

/* Helper - build UTC label string from offset in minutes                    */
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


/* Init - World Clock                                                         */
/*
 * RTC is set to Vietnam local time (UTC+7).
 * All offsets below are relative to Vietnam time:
 *   offset_minutes = (country_utc_offset_hours * 60) - (7 * 60)
 */
void mc_clock_world_clock_init(void)
{
    /* Base: Vietnam UTC+7 */
    strcpy(world_clock.country[0].name, "Vietnam");
    world_clock.country[0].timezone_offset_minutes = 0;
    build_utc_label(world_clock.country[0].utc_label, 7 * 60);

    /* UTC+0 */
    strcpy(world_clock.country[1].name, "England");
    world_clock.country[1].timezone_offset_minutes = -420;
    build_utc_label(world_clock.country[1].utc_label, 0);

    strcpy(world_clock.country[2].name, "Portugal");
    world_clock.country[2].timezone_offset_minutes = -420;
    build_utc_label(world_clock.country[2].utc_label, 0);

    /* UTC+1 */
    strcpy(world_clock.country[3].name, "France");
    world_clock.country[3].timezone_offset_minutes = -360;
    build_utc_label(world_clock.country[3].utc_label, 60);

    strcpy(world_clock.country[4].name, "Germany");
    world_clock.country[4].timezone_offset_minutes = -360;
    build_utc_label(world_clock.country[4].utc_label, 60);

    /* UTC+2 */
    strcpy(world_clock.country[5].name, "Egypt");
    world_clock.country[5].timezone_offset_minutes = -300;
    build_utc_label(world_clock.country[5].utc_label, 120);

    strcpy(world_clock.country[6].name, "Greece");
    world_clock.country[6].timezone_offset_minutes = -300;
    build_utc_label(world_clock.country[6].utc_label, 120);

    /* UTC+3 */
    strcpy(world_clock.country[7].name, "Moscow");
    world_clock.country[7].timezone_offset_minutes = -240;
    build_utc_label(world_clock.country[7].utc_label, 180);

    strcpy(world_clock.country[8].name, "Dubai");
    world_clock.country[8].timezone_offset_minutes = -180;
    build_utc_label(world_clock.country[8].utc_label, 240);

    /* UTC+3:30 */
    strcpy(world_clock.country[9].name, "Iran");
    world_clock.country[9].timezone_offset_minutes = -150;
    build_utc_label(world_clock.country[9].utc_label, 210);

    /* UTC+5 */
    strcpy(world_clock.country[10].name, "Pakistan");
    world_clock.country[10].timezone_offset_minutes = -120;
    build_utc_label(world_clock.country[10].utc_label, 300);

    /* UTC+5:30 */
    strcpy(world_clock.country[11].name, "India");
    world_clock.country[11].timezone_offset_minutes = -90;
    build_utc_label(world_clock.country[11].utc_label, 330);

    /* UTC+6 */
    strcpy(world_clock.country[12].name, "Dhaka");
    world_clock.country[12].timezone_offset_minutes = -60;
    build_utc_label(world_clock.country[12].utc_label, 360);

    /* UTC+6:30 */
    strcpy(world_clock.country[13].name, "Myanmar");
    world_clock.country[13].timezone_offset_minutes = -30;
    build_utc_label(world_clock.country[13].utc_label, 390);

    /* UTC+7 */
    strcpy(world_clock.country[14].name, "Thailand");
    world_clock.country[14].timezone_offset_minutes = 0;
    build_utc_label(world_clock.country[14].utc_label, 420);

    strcpy(world_clock.country[15].name, "Jakarta");
    world_clock.country[15].timezone_offset_minutes = 0;
    build_utc_label(world_clock.country[15].utc_label, 420);

    /* UTC+8 */
    strcpy(world_clock.country[16].name, "Singapore");
    world_clock.country[16].timezone_offset_minutes = 60;
    build_utc_label(world_clock.country[16].utc_label, 480);

    strcpy(world_clock.country[17].name, "China");
    world_clock.country[17].timezone_offset_minutes = 60;
    build_utc_label(world_clock.country[17].utc_label, 480);

    strcpy(world_clock.country[18].name, "Korea");
    world_clock.country[18].timezone_offset_minutes = 120;
    build_utc_label(world_clock.country[18].utc_label, 540);

    strcpy(world_clock.country[19].name, "Japan");
    world_clock.country[19].timezone_offset_minutes = 120;
    build_utc_label(world_clock.country[19].utc_label, 540);

    /* UTC+9:30 */
    strcpy(world_clock.country[20].name, "Adelaide");
    world_clock.country[20].timezone_offset_minutes = 150;
    build_utc_label(world_clock.country[20].utc_label, 570);

    /* UTC+10 */
    strcpy(world_clock.country[21].name, "Sydney");
    world_clock.country[21].timezone_offset_minutes = 180;
    build_utc_label(world_clock.country[21].utc_label, 600);

    /* UTC+12 */
    strcpy(world_clock.country[22].name, "Auckland");
    world_clock.country[22].timezone_offset_minutes = 300;
    build_utc_label(world_clock.country[22].utc_label, 720);

    /* UTC-1 */
    strcpy(world_clock.country[23].name, "Azores");
    world_clock.country[23].timezone_offset_minutes = -480;
    build_utc_label(world_clock.country[23].utc_label, -60);

    /* UTC-3 */
    strcpy(world_clock.country[24].name, "Brazil");
    world_clock.country[24].timezone_offset_minutes = -600;
    build_utc_label(world_clock.country[24].utc_label, -180);

    /* UTC-4 */
    strcpy(world_clock.country[25].name, "Caracas");
    world_clock.country[25].timezone_offset_minutes = -660;
    build_utc_label(world_clock.country[25].utc_label, -240);

    /* UTC-5 */
    strcpy(world_clock.country[26].name, "USA");
    world_clock.country[26].timezone_offset_minutes = -720;
    build_utc_label(world_clock.country[26].utc_label, -300);

    /* UTC-6 */
    strcpy(world_clock.country[27].name, "Chicago");
    world_clock.country[27].timezone_offset_minutes = -780;
    build_utc_label(world_clock.country[27].utc_label, -360);

    /* UTC-7 */
    strcpy(world_clock.country[28].name, "Denver");
    world_clock.country[28].timezone_offset_minutes = -840;
    build_utc_label(world_clock.country[28].utc_label, -420);

    /* UTC-8 */
    strcpy(world_clock.country[29].name, "LA");
    world_clock.country[29].timezone_offset_minutes = -900;
    build_utc_label(world_clock.country[29].utc_label, -480);

    /* UTC-9 */
    strcpy(world_clock.country[30].name, "Alaska");
    world_clock.country[30].timezone_offset_minutes = -960;
    build_utc_label(world_clock.country[30].utc_label, -540);

    /* UTC-10 */
    strcpy(world_clock.country[31].name, "Hawaii");
    world_clock.country[31].timezone_offset_minutes = -1020;
    build_utc_label(world_clock.country[31].utc_label, -600);

    world_clock.selected_country = 0;

    world_clock.hour = 0;
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