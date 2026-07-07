#include "scr_clock_timer.h"

static uint16_t timer_set_minutes = 5;
static uint16_t timer_remaining_min = 5;
static uint16_t timer_remaining_sec = 0;
static uint8_t timer_running = 0;
static uint8_t current_item = SCR_CLOCK_TIMER_SET;
static uint8_t timer_mode = 0;
static uint8_t pomodoro_round = 1;
static uint16_t total_time_elapsed = 0;
static char timer_status[20] = "STOPPED";

static void view_scr_clock_timer();
view_dynamic_t dyn_view_scr_clock_timer = {
    { .item_type = ITEM_TYPE_DYNAMIC, },
    view_scr_clock_timer
};

view_screen_t scr_clock_timer = {
    &dyn_view_scr_clock_timer,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

void view_scr_clock_timer()
{
    view_render.clear();
    view_render.drawRoundRect(0, 0, 128, 64, 6, WHITE);

    view_render.setCursor(28, 3);
    view_render.setTextSize(2);
    view_render.setTextColor(WHITE);
    view_render.print("TIMER");

    view_render.setCursor(18, 26);
    view_render.setTextSize(3);
    char time_buf[8];
    sprintf(time_buf, "%02d:%02d", timer_remaining_min, timer_remaining_sec);
    view_render.print(time_buf);

    view_render.setCursor(5, 52);
    view_render.setTextSize(1);
    view_render.print(timer_status);

    view_render.setCursor(95, 5);
    view_render.setTextSize(1);
    if (timer_mode == 1) view_render.print("RPT");
    else if (timer_mode == 2) view_render.print("POMO");

    uint8_t progress = (timer_set_minutes > 0) ?
                       ((timer_set_minutes - timer_remaining_min) * 100 / timer_set_minutes) : 0;
    view_render.fillRect(5, 60, (progress * 118 / 100), 3, WHITE);
}

static void timer_update_status()
{
    if (timer_running == 1)
        strcpy(timer_status, "RUNNING");
    else if (timer_running == 2)
        strcpy(timer_status, "PAUSED");
    else
        strcpy(timer_status, "STOPPED");
}

static void timer_reset()
{
    timer_running = 0;
    timer_remaining_min = timer_set_minutes;
    timer_remaining_sec = 0;
    total_time_elapsed = 0;
    timer_update_status();
    BUZZER_PlaySound(BUZZER_SOUND_USB_DISCONNECTED);
}

static void timer_beep_finish()
{
    APP_DBG("[TIMER] === TIME UP! ===\n");
    BUZZER_PlaySound(BUZZER_SOUND_GOODBYE);
}

void scr_clock_timer_handle(ak_msg_t* msg)
{
    switch (msg->sig)
    {
    case SCREEN_ENTRY:
        current_item = SCR_CLOCK_TIMER_SET;
        timer_update_status();
        break;

    case AC_DISPLAY_BUTON_MODE_PRESSED:
        if (current_item == SCR_CLOCK_TIMER_BACK) {
            SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
            return;
        }
        if (timer_running == 0) {
            if (timer_remaining_min == 0 && timer_remaining_sec == 0)
                timer_remaining_min = timer_set_minutes;
            timer_running = 1;
        } else if (timer_running == 1) {
            timer_running = 2;
        } else {
            timer_running = 1;
        }
        timer_update_status();
        break;

    case AC_DISPLAY_BUTON_LONG_MODE_PRESSED:
        timer_reset();
        APP_DBG("[TIMER] Full Reset\n");
        BUZZER_PlaySound(BUZZER_SOUND_USB_DISCONNECTED);
        break;

    case AC_DISPLAY_BUTON_UP_PRESSED:
        if (timer_running == 0) {
            timer_set_minutes += 5;
            if (timer_set_minutes > SCR_CLOCK_TIMER_MAX_MINUTES)
                timer_set_minutes = SCR_CLOCK_TIMER_MAX_MINUTES;
            timer_remaining_min = timer_set_minutes;
        } else {
            timer_remaining_sec += 10;
            if (timer_remaining_sec >= 60) {
                timer_remaining_sec = 0;
                timer_remaining_min++;
            }
        }
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        break;

    case AC_DISPLAY_BUTON_DOWN_PRESSED:
        if (timer_running == 0) {
            if (timer_set_minutes >= 5) timer_set_minutes -= 5;
            timer_remaining_min = timer_set_minutes;
        } else {
            if (timer_remaining_sec >= 10)
                timer_remaining_sec -= 10;
            else if (timer_remaining_min > 0) {
                timer_remaining_min--;
                timer_remaining_sec = 50;
            }
        }
        BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        break;

    case AC_DISPLAY_BUTON_LONG_UP_PRESSED:
    case AC_DISPLAY_BUTON_LONG_DOWN_PRESSED:
        SCREEN_TRAN(scr_clock_menu_handle, &scr_clock_menu);
        break;

    default:
        break;
    }
}

void timer_background_update(void)
{
    if (timer_running != 1) return;

    total_time_elapsed++;

    if (timer_remaining_sec == 0) {
        if (timer_remaining_min == 0) {
            timer_running = 0;
            timer_beep_finish();
            timer_reset();
            if (timer_mode == 1) {
                timer_running = 1;
            }
        } else {
            timer_remaining_min--;
            timer_remaining_sec = 59;
        }
    } else {
        timer_remaining_sec--;
    }

    timer_update_status();
}