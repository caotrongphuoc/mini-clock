#include "scr_clock_alarm.h"

static uint8_t current_item = SCR_CLOCK_ALARM_LIST;
static uint8_t selected_alarm = 0;

static void view_scr_clock_alarm();
view_dynamic_t dyn_view_scr_clock_alarm = {
    { .item_type = ITEM_TYPE_DYNAMIC, },
    view_scr_clock_alarm
};

view_screen_t scr_clock_alarm = {
    &dyn_view_scr_clock_alarm,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void scr_clock_alarm_draw_item(uint8_t index)
{
    uint8_t y = index * 12;
    uint16_t bg = (index == current_item) ? WHITE : BLACK;
    uint16_t fg = (index == current_item) ? BLACK : WHITE;

    view_render.fillRect(0, y, 128, 12, bg);

    view_render.setTextSize(1);
    view_render.setTextColor(fg);

    switch (index)
    {
    case 0:
        view_render.setCursor(5, y + 2);
        view_render.print("1. Alarm 1  07:30");
        break;
    case 1:
        view_render.setCursor(5, y + 2);
        view_render.print("2. Alarm 2  08:00");
        break;
    case 2:
        view_render.setCursor(5, y + 2);
        view_render.print("3. Alarm 3  --:--");
        break;
    case 3:
        view_render.setCursor(5, y + 2);
        view_render.print("Add New Alarm");
        break;
    case 4:
        view_render.setCursor(5, y + 2);
        view_render.print("Back to Menu");
        break;
    }
}

void view_scr_clock_alarm()
{
    view_render.clear();
    for (uint8_t i = 0; i < 5; i++)
    {
        scr_clock_alarm_draw_item(i);
    }
}

void scr_clock_alarm_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
        current_item = SCR_CLOCK_ALARM_LIST;
        break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
    {
        switch (current_item)
        {
        case 4:  // Back
            SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
            break;

        case 3:  // Add New Alarm
            APP_DBG("[ALARM] Add new alarm selected\n");
            break;

        default: // Select existing alarm
            selected_alarm = current_item;
            APP_DBG("[ALARM] Selected alarm %u\n", selected_alarm);
            // TODO: Go to edit screen later
            break;
        }
    }
    break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
        if (current_item == 0) current_item = 4;
        else current_item--;
        break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
        current_item++;
        if (current_item >= 5) current_item = 0;
        break;

    default:
        break;
    }
}