#include "scr_clock_menu.h"

#include "mc_clock_time.h"
#include "scr_clock_main.h"
#include "scr_clock_setting_display.h"

/*****************************************************************************/
/* Variable and Struct Declaration - Clock menu */
/*****************************************************************************/
static uint8_t current_location = SCR_CLOCK_MENU_CLOCK;

static const unsigned char* const menu_bitmap[SCR_CLOCK_MENU_ITEM_NUMBER] = {
    bitmap_clock_menu_clock,
    bitmap_clock_menu_alarm,
    bitmap_clock_menu_stopwatch,
    bitmap_clock_menu_timer,
    bitmap_clock_menu_setting,
    bitmap_clock_menu_exit,
};

/*****************************************************************************/
/* View - Clock menu */
/*****************************************************************************/
static void view_scr_clock_menu();
view_dynamic_t dyn_view_scr_clock_menu = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_menu};

view_screen_t scr_clock_menu = {
    &dyn_view_scr_clock_menu,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_clock_menu_draw_icon()
{
    view_render.drawBitmap(SCR_CLOCK_MENU_ICON_X,
                           SCR_CLOCK_MENU_ICON_Y,
                           menu_bitmap[current_location],
                           SCR_CLOCK_MENU_ICON_W,
                           SCR_CLOCK_MENU_ICON_H,
                           WHITE);
}

void scr_clock_menu_draw_time()
{
    mc_clock_time_state_t state;
    char time_text[12];
    uint8_t use_12h = scr_clock_setting_is_12h_format();

    mc_clock_time_get_state(&state);
    scr_clock_main_format_time(time_text, &state.time, use_12h);

    if (use_12h)
    {
        time_text[8] = ' ';
        time_text[9] = (state.time.hour >= 12) ? 'P' : 'A';
        time_text[10] = 'M';
        time_text[11] = '\0';
    }

    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    view_render.setCursor(use_12h ? 31 : 40, 3);
    view_render.print(time_text);
}

void view_scr_clock_menu()
{
    view_render.clear();
    view_render.drawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
    scr_clock_menu_draw_time();
    scr_clock_menu_draw_icon();
}

/*****************************************************************************/
/* Handle - Clock menu */
/*****************************************************************************/
void scr_clock_menu_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
    {
        APP_DBG_SIG("SCREEN_ENTRY\n");
        current_location = SCR_CLOCK_MENU_CLOCK;
    }
    break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
        switch (current_location)
        {
        case SCR_CLOCK_MENU_CLOCK:
            SCREEN_TRAN(scr_clock_main_handle, &scr_clock_main);
            break;

        case SCR_CLOCK_MENU_ALARM:
            SCREEN_TRAN(scr_clock_alarm_handle, &scr_clock_alarm);
            break;

        case SCR_CLOCK_MENU_STOPWATCH:
            SCREEN_TRAN(scr_clock_stopwatch_handle, &scr_clock_stopwatch);
            break;

        case SCR_CLOCK_MENU_TIMER:
            SCREEN_TRAN(scr_clock_timer_handle, &scr_clock_timer);
            break;

        case SCR_CLOCK_MENU_SETTING:
            SCREEN_TRAN(scr_clock_setting_handle, &scr_clock_setting);
            break;

        case SCR_CLOCK_MENU_EXIT:
            SCREEN_TRAN(scr_idle_handle, &scr_idle);
            break;

        default:
            APP_DBG("[MENU] item %u is not ready\n", current_location);
            break;
        }
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
    {
        APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
        if (current_location == 0)
        {
            current_location = SCR_CLOCK_MENU_ITEM_NUMBER - 1;
        }
        else
        {
            current_location--;
        }
    }
    break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
    {
        APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
        current_location++;
        if (current_location >= SCR_CLOCK_MENU_ITEM_NUMBER)
        {
            current_location = 0;
        }
    }
    break;

    default:
        break;
    }
}
