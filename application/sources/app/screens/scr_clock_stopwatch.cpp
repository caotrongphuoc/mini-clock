#include "scr_clock_stopwatch.h"
#include "screens.h"
#include <stdio.h>

static uint8_t  current_state = 0;
static uint8_t  current_item  = SCR_CLOCK_STOPWATCH_DISPLAY;
static uint32_t stopwatch_ms  = 0;

static void view_scr_clock_stopwatch();
view_dynamic_t dyn_view_scr_clock_stopwatch = {
    { .item_type = ITEM_TYPE_DYNAMIC, },
    view_scr_clock_stopwatch
};

view_screen_t scr_clock_stopwatch = {
    &dyn_view_scr_clock_stopwatch,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void view_scr_clock_stopwatch()
{
    view_render.clear();
    view_render.drawRoundRect(0, 0, 128, 64, 5, WHITE);

    view_render.setCursor(10, 2);
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.print("STOPWATCH");

    view_render.setCursor(20, 25);
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.print("State: ");
    view_render.setCursor(70, 25);
    view_render.print(current_state ? "RUNNING" : "STOPPED");

    uint32_t total_seconds = stopwatch_ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;
    uint32_t ms = (stopwatch_ms % 1000) / 10;

    view_render.setCursor(12, 42);
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);

    char time_str[12];
    sprintf(time_str, "%02lu:%02lu:%02lu", minutes, seconds, ms);
    view_render.print(time_str);
}

void scr_clock_stopwatch_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
        current_item = SCR_CLOCK_STOPWATCH_DISPLAY;
        break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
        if (current_item == SCR_CLOCK_STOPWATCH_BACK) {
            SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
        } else {
            current_state = !current_state;
            APP_DBG("[STOPWATCH] %s\n", current_state ? "STARTED" : "PAUSED");
        }
        break;

    case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
        current_state = 0;
        stopwatch_ms = 0;
        APP_DBG("[STOPWATCH] Reset to 0\n");
        break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
        if (current_state == 1) {
            APP_DBG("[STOPWATCH] Lap recorded\n");
        } else {
            if (current_item == 0) current_item = 5;
            else current_item--;
        }
        break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
        if (current_state == 1) {
            // Optional
        } else {
            current_item++;
            if (current_item >= 6) current_item = 0;
        }
        break;

    case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
    case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
        SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
        break;

    default:
        break;
    }
}

/* Call this every 10ms from your system timer */
void stopwatch_background_update(void)
{
    if (current_state == 1) {
        stopwatch_ms += 10;
    }
}