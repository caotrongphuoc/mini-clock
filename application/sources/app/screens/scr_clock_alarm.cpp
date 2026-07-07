#include "scr_clock_alarm.h"
#include "screens.h"
#include <stdio.h>

#define MAX_ALARMS 10

static uint8_t current_item = 0;
static uint8_t scroll_offset = 0;
static uint8_t total_alarms = 3;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t enabled;
} alarm_t;

static alarm_t alarms[MAX_ALARMS] = {
    {7, 30, 1},
    {8, 0, 1},
    {12, 15, 0}
};

static uint8_t editing_mode = 0;      // 0 = list, 1 = editing
static uint8_t editing_alarm = 0;
static uint8_t editing_field = 0;     // 0 = hour, 1 = minute

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

/* View */
void view_scr_clock_alarm()
{
    view_render.clear();

    if (editing_mode == 1) {
        // === Editing Mode ===
        view_render.drawRoundRect(0, 0, 128, 64, 5, WHITE);

        view_render.setCursor(5, 5);
        view_render.setTextSize(2);
        view_render.setTextColor(WHITE);
        view_render.print("Edit Alarm");

        char buf[10];
        sprintf(buf, "%02d:%02d", alarms[editing_alarm].hour, alarms[editing_alarm].minute);
        
        view_render.setCursor(15, 25);
        view_render.setTextSize(3);
        view_render.print(buf);

        view_render.setCursor(10, 53);
        view_render.setTextSize(1);
        view_render.print("Press MODE to save");


    } else {
        // === List Mode ===
        uint8_t visible_lines = 3;

        for (uint8_t i = 0; i < visible_lines; i++) {
            uint8_t alarm_idx = scroll_offset + i;
            if (alarm_idx >= total_alarms) break;

            uint8_t y = i * 13;
            uint16_t bg = (alarm_idx == current_item) ? WHITE : BLACK;
            uint16_t fg = (alarm_idx == current_item) ? BLACK : WHITE;

            view_render.fillRect(0, y, 128, 13, bg);
            view_render.setTextSize(1);
            view_render.setTextColor(fg);

            char line[22];
            sprintf(line, "%d. %02d:%02d %s", alarm_idx+1, 
                    alarms[alarm_idx].hour, alarms[alarm_idx].minute,
                    alarms[alarm_idx].enabled ? "[ON]" : "[OFF]");

            view_render.setCursor(4, y + 3);
            view_render.print(line);
        }

        // Fixed Bottom Bar
        view_render.fillRect(0, 40, 128, 24, BLACK);

        // Add New
        view_render.fillRoundRect(4, 43, 70, 14, 3, current_item == total_alarms ? WHITE : BLACK);
        view_render.setTextColor(current_item == total_alarms ? BLACK : WHITE);
        view_render.setCursor(10, 47);
        view_render.print("Add New");

        // Back
        view_render.fillRoundRect(80, 43, 45, 14, 3, current_item == total_alarms + 1 ? WHITE : BLACK);
        view_render.setTextColor(current_item == total_alarms + 1 ? BLACK : WHITE);
        view_render.setCursor(88, 47);
        view_render.print("Back");
    }
}

/* Handle */
void scr_clock_alarm_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
        current_item = 0;
        editing_mode = 0;
        break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
        if (editing_mode == 1) {
            editing_mode = 0;                    // Exit editing
        }
        else if (current_item == total_alarms) { // Add New
            if (total_alarms < MAX_ALARMS) {
                alarms[total_alarms].hour = 8;
                alarms[total_alarms].minute = 0;
                alarms[total_alarms].enabled = 1;
                total_alarms++;
                current_item = total_alarms - 1;
            }
        }
        else if (current_item == total_alarms + 1) { // Back
            SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
        }
        else {
            editing_mode = 1;
            editing_alarm = current_item;
            editing_field = 0;
        }
        break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
        if (editing_mode == 1) {
            if (editing_field == 0) { // Hour
                alarms[editing_alarm].hour = (alarms[editing_alarm].hour + 1) % 24;
            } else { // Minute
                alarms[editing_alarm].minute = (alarms[editing_alarm].minute + 1) % 60;
            }
        } else {
            if (current_item > 0) current_item--;
            if (current_item < scroll_offset) scroll_offset = current_item;
        }
        break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
        if (editing_mode == 1) {
            if (editing_field == 0) {
                if (alarms[editing_alarm].hour > 0) alarms[editing_alarm].hour--;
                else alarms[editing_alarm].hour = 23;
            } else {
                if (alarms[editing_alarm].minute > 0) alarms[editing_alarm].minute--;
                else alarms[editing_alarm].minute = 59;
            }
        } else {
            current_item++;
            if (current_item > total_alarms + 1) current_item = total_alarms + 1;
            if (current_item >= scroll_offset + 3)
                scroll_offset = current_item - 2;
        }
        break;

    default:
        break;
    }
}