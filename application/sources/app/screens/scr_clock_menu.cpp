#include "scr_clock_menu.h"

static uint8_t s_scr_clock_menu_focus = SCR_CLOCK_MENU_CLOCK;

static const uint8_t s_scr_clock_menu_bitmap_clock[SCR_CLOCK_MENU_ICON_BYTES] = {
    0,
};

static const uint8_t s_scr_clock_menu_bitmap_alarm[SCR_CLOCK_MENU_ICON_BYTES] = {
    0,
};

static const uint8_t s_scr_clock_menu_bitmap_stopwatch[SCR_CLOCK_MENU_ICON_BYTES] = {
    0,
};

static const uint8_t s_scr_clock_menu_bitmap_timer[SCR_CLOCK_MENU_ICON_BYTES] = {
    0,
};

static const uint8_t s_scr_clock_menu_bitmap_setting[SCR_CLOCK_MENU_ICON_BYTES] = {
    0,
};

static const uint8_t s_scr_clock_menu_bitmap_exit[SCR_CLOCK_MENU_ICON_BYTES] = {
    0,
};

static const uint8_t* const s_scr_clock_menu_bitmaps[SCR_CLOCK_MENU_ITEM_NUMBER] = {
    s_scr_clock_menu_bitmap_clock,
    s_scr_clock_menu_bitmap_alarm,
    s_scr_clock_menu_bitmap_stopwatch,
    s_scr_clock_menu_bitmap_timer,
    s_scr_clock_menu_bitmap_setting,
    s_scr_clock_menu_bitmap_exit,
};

static void view_scr_clock_menu();
static void scr_clock_menu_draw_item(uint8_t index);
static void scr_clock_menu_focus_next();
static void scr_clock_menu_focus_prev();
static void scr_clock_menu_select();

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

static void view_scr_clock_menu()
{
	view_render.clear();

	for (uint8_t i = 0; i < SCR_CLOCK_MENU_ITEM_NUMBER; i++)
	{
		scr_clock_menu_draw_item(i);
	}
}

static void scr_clock_menu_draw_item(uint8_t index)
{
	uint8_t col = index % SCR_CLOCK_MENU_COL_NUMBER;
	uint8_t row = index / SCR_CLOCK_MENU_COL_NUMBER;
	int16_t x = col * SCR_CLOCK_MENU_CELL_W;
	int16_t y = row * SCR_CLOCK_MENU_CELL_H;
	uint16_t bg_color = (index == s_scr_clock_menu_focus) ? WHITE : BLACK;
	uint16_t icon_color = (index == s_scr_clock_menu_focus) ? BLACK : WHITE;
	int16_t icon_x = x + ((SCR_CLOCK_MENU_CELL_W - SCR_CLOCK_MENU_ICON_W) / 2);
	int16_t icon_y = y + ((SCR_CLOCK_MENU_CELL_H - SCR_CLOCK_MENU_ICON_H) / 2);

	if (index >= SCR_CLOCK_MENU_ITEM_NUMBER)
	{
		return;
	}

	view_render.fillRect(x, y, SCR_CLOCK_MENU_CELL_W, SCR_CLOCK_MENU_CELL_H, bg_color);
	view_render.drawBitmap(icon_x, icon_y, s_scr_clock_menu_bitmaps[index], SCR_CLOCK_MENU_ICON_W, SCR_CLOCK_MENU_ICON_H, icon_color);
}

static void scr_clock_menu_focus_next()
{
	s_scr_clock_menu_focus++;
	if (s_scr_clock_menu_focus >= SCR_CLOCK_MENU_ITEM_NUMBER)
	{
		s_scr_clock_menu_focus = 0;
	}
}

static void scr_clock_menu_focus_prev()
{
	if (s_scr_clock_menu_focus == 0)
	{
		s_scr_clock_menu_focus = SCR_CLOCK_MENU_ITEM_NUMBER - 1;
	}
	else
	{
		s_scr_clock_menu_focus--;
	}
}

static void scr_clock_menu_select()
{
	switch (s_scr_clock_menu_focus)
	{
	case SCR_CLOCK_MENU_CLOCK:
		SCREEN_BACK();
		break;

	case SCR_CLOCK_MENU_SETTING:
		APP_DBG("[MENU] setting selected\n");
		break;

	case SCR_CLOCK_MENU_EXIT:
		SCREEN_BACK();
		break;

	case SCR_CLOCK_MENU_ALARM:
	case SCR_CLOCK_MENU_STOPWATCH:
	case SCR_CLOCK_MENU_TIMER:
	default:
		APP_DBG("[MENU] item %u is not ready\n", s_scr_clock_menu_focus);
		break;
	}
}

void scr_clock_menu_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("SCREEN_ENTRY\n");
		s_scr_clock_menu_focus = SCR_CLOCK_MENU_CLOCK;
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
		scr_clock_menu_select();
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
		scr_clock_menu_focus_prev();
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
		scr_clock_menu_focus_next();
	}
	break;

	default:
		break;
	}
}
