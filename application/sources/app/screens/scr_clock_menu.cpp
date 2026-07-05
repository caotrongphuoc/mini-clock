#include "scr_clock_menu.h"

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

void scr_clock_menu_draw_item(uint8_t index)
{
	uint8_t col = index % SCR_CLOCK_MENU_COL_NUMBER;
	uint8_t row = index / SCR_CLOCK_MENU_COL_NUMBER;
	int16_t x = col * SCR_CLOCK_MENU_CELL_W;
	int16_t y = row * SCR_CLOCK_MENU_CELL_H;
	int16_t icon_x = x + ((SCR_CLOCK_MENU_CELL_W - SCR_CLOCK_MENU_ICON_W) / 2);
	int16_t icon_y = y + ((SCR_CLOCK_MENU_CELL_H - SCR_CLOCK_MENU_ICON_H) / 2);
	uint16_t bg = (index == current_location) ? WHITE : BLACK;
	uint16_t fg = (index == current_location) ? BLACK : WHITE;

	view_render.fillRect(x, y, SCR_CLOCK_MENU_CELL_W, SCR_CLOCK_MENU_CELL_H, bg);
	view_render.drawBitmap(icon_x, icon_y, menu_bitmap[index], SCR_CLOCK_MENU_ICON_W, SCR_CLOCK_MENU_ICON_H, fg);
}

void view_scr_clock_menu()
{
	view_render.clear();

	for (uint8_t i = 0; i < SCR_CLOCK_MENU_ITEM_NUMBER; i++)
	{
		scr_clock_menu_draw_item(i);
	}
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

		case SCR_CLOCK_MENU_SETTING:
			SCREEN_TRAN(scr_clock_setting_handle, &scr_clock_setting);
			break;

		case SCR_CLOCK_MENU_EXIT:
			SCREEN_TRAN(scr_clock_main_handle, &scr_clock_main);
			break;

		case SCR_CLOCK_MENU_ALARM:
		case SCR_CLOCK_MENU_STOPWATCH:
		case SCR_CLOCK_MENU_TIMER:
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
