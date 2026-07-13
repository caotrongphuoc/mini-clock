#include "scr_world_clock.h"

#include "mc_clock_world_clock.h"
#include "task_list.h"
#include "timer.h"

static void view_scr_world_clock();

view_dynamic_t dyn_view_scr_world_clock = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_world_clock};

view_screen_t scr_world_clock = {
    &dyn_view_scr_world_clock,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};


static void view_scr_world_clock()
{
    mc_world_clock_state_t state;

    mc_clock_world_clock_get_state(&state);

    const mc_world_clock_country_t* country =
            &state.country[state.selected_country];

    char time_buf[12];
    xsprintf(time_buf, "%02u:%02u:%02u",
             state.hour, state.minute, state.second);

    /* Country index label e.g. " 1/10" */
    char idx_buf[8];
    xsprintf(idx_buf, "%u/%u",
             state.selected_country + 1,
             MC_WORLD_CLOCK_MAX_COUNTRY);

    view_render.clear();
    view_render.setTextColor(WHITE);

    /* ── Border ── */
    view_render.drawRect(0, 0, 128, 64, WHITE);

    /* ── Header: "World Clock" ── */
    view_render.setTextSize(1);
    view_render.setCursor(30, 3);
    view_render.print("World Clock");

    view_render.drawLine(0, 12, 128, 12, WHITE);

    /* ── Country name ── */
    view_render.setTextSize(1);
    view_render.setCursor(4, 16);
    view_render.print(country->name);

    /* ── Country index (right-aligned) ── */
    uint8_t idx_len = 0;
    const char* p = idx_buf;
    while (*p++) idx_len++;
    view_render.setCursor(124 - idx_len * 6, 16);
    view_render.print(idx_buf);

    /* ── Time display (large) ── */
    view_render.setTextSize(2);
    view_render.setCursor(8, 28);
    view_render.print(time_buf);

    /* ── UTC label ── */
    view_render.setTextSize(1);
    view_render.setCursor(55, 16);
    view_render.print(country->utc_label);

    /* ── Navigation hint ── */
    view_render.setCursor(15, 52);
    view_render.print("UP");
    view_render.setCursor(50, 52);
    view_render.print("DOWN");
    view_render.setCursor(100, 52);
    view_render.print("MODE");
}

void scr_world_clock_handle(
        ak_msg_t* msg)
{

    switch(msg->sig)
    {

        case SCREEN_ENTRY:
        {
            APP_DBG_SIG("SCREEN_ENTRY [world clock]\n");

            /* Initialize and compute time for current country */
            task_post_pure_msg(MC_CLOCK_WORLD_CLOCK_ID, MC_CLOCK_WORLD_CLOCK_SETUP);

            /* Reuse the 1-second tick from the time module */
            timer_set(AC_TASK_DISPLAY_ID,
                      MC_CLOCK_TIME_TICK,
                      MC_CLOCK_TIME_TICK_INTERVAL,
                      TIMER_PERIODIC);
        }
        break;


        case MC_CLOCK_TIME_TICK:
        {
            /* Every second: update the world clock state then re-render */
            task_post_pure_msg(MC_CLOCK_WORLD_CLOCK_ID, MC_CLOCK_WORLD_CLOCK_UPDATE);
        }
        break;


        case AC_DISPLAY_BUTON_DOWN_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED [world clock]\n");
            task_post_pure_msg(MC_CLOCK_WORLD_CLOCK_ID, MC_CLOCK_WORLD_CLOCK_SELECT_DOWN);
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }
        break;


        case AC_DISPLAY_BUTON_UP_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED [world clock]\n");
            task_post_pure_msg(MC_CLOCK_WORLD_CLOCK_ID, MC_CLOCK_WORLD_CLOCK_SELECT_UP);
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }
        break;


        case AC_DISPLAY_BUTON_MODE_PRESSED:
        {
            APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED [world clock]\n");
            timer_remove_attr(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
            SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
        }
        break;


        default:
        break;

    }


}