#include "mc_time_change.h"

// Forward declaration
static void view_mc_time_change();

view_dynamic_t dyn_view_mc_time_change = {
    { .item_type = ITEM_TYPE_DYNAMIC, },
    view_mc_time_change
};

view_screen_t mc_time_change = {
    &dyn_view_mc_time_change,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

struct country_time_zone {
    const char* name;
    int offset_hours;
};

struct country_time_zone time_zones[total_time_zone] = {
    {"UTC-4", -4},
    {"UTC-3", -3},
    {"UTC-2", -2},
    {"UTC-1", -1},
    {"UTC+0 (London)", 0},
    {"UTC+1 (Paris)", 1},
    {"UTC+7 (Jakarta)", 7},
    {"UTC+8 (Singapore)", 8},
};

static uint8_t current_tz_index = 5;   // default index

extern uint8_t global_hour;
extern uint8_t global_minute;
extern uint8_t global_day;
extern uint8_t global_month;

static void view_mc_time_change()
{
    view_render.clear();

    view_render.setCursor(5, 5);
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.print("Time Zone");

    view_render.setCursor(5, 28);
    view_render.setTextSize(1);
    view_render.print(time_zones[current_tz_index].name);

    int adjusted_hour = global_hour + time_zones[current_tz_index].offset_hours;
    if (adjusted_hour >= 24) adjusted_hour -= 24;
    if (adjusted_hour < 0) adjusted_hour += 24;

    view_render.setCursor(20, 42);
    view_render.setTextSize(2);
    char buf[12];
    sprintf(buf, "%02d:%02d", adjusted_hour, global_minute);
    view_render.print(buf);

    view_render.setCursor(5, 55);
    view_render.setTextSize(1);
    sprintf(buf, "Date: %02d/%02d", global_day, global_month);
    view_render.print(buf);
}

void mc_time_change_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
        current_tz_index = current_time_zone;
        break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
        current_time_zone = current_tz_index;   // Save selection
        SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
        break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
        if (current_tz_index > 0) current_tz_index--;
        break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
        if (current_tz_index < total_time_zone - 1) current_tz_index++;
        break;
    
    case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
    case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
    case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
        SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
        break;

    default:
        break;
    }
}