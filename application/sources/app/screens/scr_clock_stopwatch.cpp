#include "scr_clock_stopwatch.h"
#include "screens.h"
#include "app_dbg.h"
#include <stdio.h>

static uint8_t current_state = 0;
static uint8_t current_item = SCR_CLOCK_STOPWATCH_DISPLAY;

static uint32_t stopwatch_seconds = 0;

static void view_scr_clock_stopwatch();

view_dynamic_t dyn_view_scr_clock_stopwatch =
{
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_stopwatch
};


view_screen_t scr_clock_stopwatch =
{
    &dyn_view_scr_clock_stopwatch,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};



void view_scr_clock_stopwatch()
{
    view_render.clear();

    view_render.drawRoundRect(
        0,
        0,
        128,
        64,
        5,
        WHITE);


    view_render.setCursor(10,2);
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);

    view_render.print("STOPWATCH");


    view_render.setTextSize(1);

    view_render.setCursor(15,25);
    view_render.print("STATE:");

    view_render.setCursor(60,25);

    if(current_state)
        view_render.print("RUNNING");
    else
        view_render.print("STOPPED");


    uint32_t minutes = stopwatch_seconds / 60;
    uint32_t seconds = stopwatch_seconds % 60;


    char buf[12];

    sprintf(
        buf,
        "%02lu:%02lu",
        minutes,
        seconds);


    view_render.setCursor(20,45);
    view_render.setTextSize(2);

    view_render.print(buf);
}

void scr_clock_stopwatch_handle(ak_msg_t* msg)
{

    switch(msg->sig)
    {

    case SCREEN_ENTRY:
    {
        current_item = SCR_CLOCK_STOPWATCH_DISPLAY;

        current_state = 0;

        stopwatch_seconds = 0;

        view_render_display_on();

        view_scr_clock_stopwatch();
    }
    break;

    /*
     * THIS MUST COME FROM RTC
     */
    case MC_CLOCK_TIME_TICK:
    {

        if(current_state)
        {
            stopwatch_seconds++;

            view_scr_clock_stopwatch();

            APP_DBG("[STOPWATCH] %lu sec\n",
                    stopwatch_seconds);
        }

    }
    break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {

        current_state = !current_state;


        if(current_state)
        {
            APP_DBG("[STOPWATCH] START\n");
        }
        else
        {
            APP_DBG("[STOPWATCH] STOP\n");
        }

    }
    break;
    case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
    {

        current_state = 0;

        stopwatch_seconds = 0;

        view_scr_clock_stopwatch();

        APP_DBG("[STOPWATCH] RESET\n");

    }
    break;

    case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
    case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
    {
        SCREEN_TRAN(
            scr_clock_menu_handle,
            &scr_clock_menu);
    }
    break;



    default:
        break;
    }
}