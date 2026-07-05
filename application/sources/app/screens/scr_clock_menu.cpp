#include "scr_clock_menu.h"

uint8_t scr_clock_menu_focus = SCR_CLOCK_MENU_CLOCK;

const unsigned char* const scr_clock_menu_bitmaps[SCR_CLOCK_MENU_ITEM_NUMBER] = {
    bitmap_clock_menu_clock,
    bitmap_clock_menu_alarm,
    bitmap_clock_menu_stopwatch,
    bitmap_clock_menu_timer,
    bitmap_clock_menu_setting,
    bitmap_clock_menu_exit,
};

void view_scr_clock_menu();
void scr_clock_menu_draw_item(uint8_t index);
void scr_clock_menu_focus_next();
void scr_clock_menu_focus_prev();
void scr_clock_menu_select();

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

void view_scr_clock_menu()
{
	view_render.clear();

	for (uint8_t i = 0; i < SCR_CLOCK_MENU_ITEM_NUMBER; i++)
	{
		scr_clock_menu_draw_item(i);
	}
}

void scr_clock_menu_draw_item(uint8_t index)
{
	uint8_t col = index % SCR_CLOCK_MENU_COL_NUMBER;
	uint8_t row = index / SCR_CLOCK_MENU_COL_NUMBER;
	int16_t x = col * SCR_CLOCK_MENU_CELL_W;
	int16_t y = row * SCR_CLOCK_MENU_CELL_H;
	uint16_t bg_color = (index == scr_clock_menu_focus) ? WHITE : BLACK;
	uint16_t icon_color = (index == scr_clock_menu_focus) ? BLACK : WHITE;
	int16_t icon_x = x + ((SCR_CLOCK_MENU_CELL_W - SCR_CLOCK_MENU_ICON_W) / 2);
	int16_t icon_y = y + ((SCR_CLOCK_MENU_CELL_H - SCR_CLOCK_MENU_ICON_H) / 2);

	if (index >= SCR_CLOCK_MENU_ITEM_NUMBER)
	{
		return;
	}

	view_render.fillRect(x, y, SCR_CLOCK_MENU_CELL_W, SCR_CLOCK_MENU_CELL_H, bg_color);
	view_render.drawBitmap(icon_x, icon_y, scr_clock_menu_bitmaps[index], SCR_CLOCK_MENU_ICON_W, SCR_CLOCK_MENU_ICON_H, icon_color);
}

void scr_clock_menu_focus_next()
{
	scr_clock_menu_focus++;
	if (scr_clock_menu_focus >= SCR_CLOCK_MENU_ITEM_NUMBER)
	{
		scr_clock_menu_focus = 0;
	}
}

void scr_clock_menu_focus_prev()
{
	if (scr_clock_menu_focus == 0)
	{
		scr_clock_menu_focus = SCR_CLOCK_MENU_ITEM_NUMBER - 1;
	}
	else
	{
		scr_clock_menu_focus--;
	}
}

void scr_clock_menu_select()
{
	switch (scr_clock_menu_focus)
	{
	case SCR_CLOCK_MENU_CLOCK:
		SCREEN_BACK();
		break;

	case SCR_CLOCK_MENU_SETTING:
		SCREEN_TRAN(scr_clock_setting_handle, &scr_clock_setting);
		break;

	case SCR_CLOCK_MENU_EXIT:
		SCREEN_BACK();
		break;

	case SCR_CLOCK_MENU_ALARM:
	case SCR_CLOCK_MENU_STOPWATCH:
	case SCR_CLOCK_MENU_TIMER:
	default:
		APP_DBG("[MENU] item %u is not ready\n", scr_clock_menu_focus);
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
		scr_clock_menu_focus = SCR_CLOCK_MENU_CLOCK;
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
