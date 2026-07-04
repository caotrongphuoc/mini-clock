#include "scr_clock_menu.h"

static uint8_t s_scr_clock_menu_focus = SCR_CLOCK_MENU_CLOCK;

static void view_scr_clock_menu();
static void scr_clock_menu_draw_item(uint8_t index);
static void scr_clock_menu_draw_icon(uint8_t index, int16_t x, int16_t y);
static const char* scr_clock_menu_item_label(uint8_t index);
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
	view_render.setTextColor(WHITE);
	view_render.setTextSize(1);

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

	if (index == s_scr_clock_menu_focus)
	{
		view_render.drawRect(x, y, SCR_CLOCK_MENU_CELL_W, SCR_CLOCK_MENU_CELL_H, WHITE);
	}

	scr_clock_menu_draw_icon(index, x + 21, y + SCR_CLOCK_MENU_ICON_Y_OFFSET + 7);
	view_render.setCursor(x + 3, y + SCR_CLOCK_MENU_LABEL_Y_OFFSET);
	view_render.print(scr_clock_menu_item_label(index));
}

static void scr_clock_menu_draw_icon(uint8_t index, int16_t x, int16_t y)
{
	switch (index)
	{
	case SCR_CLOCK_MENU_CLOCK:
		view_render.drawCircle(x, y, 7, WHITE);
		view_render.drawLine(x, y, x, y - 5, WHITE);
		view_render.drawLine(x, y, x + 4, y + 2, WHITE);
		break;

	case SCR_CLOCK_MENU_ALARM:
		view_render.drawCircle(x, y + 1, 6, WHITE);
		view_render.drawLine(x - 6, y - 6, x - 3, y - 9, WHITE);
		view_render.drawLine(x + 6, y - 6, x + 3, y - 9, WHITE);
		view_render.drawLine(x - 4, y + 8, x - 7, y + 11, WHITE);
		view_render.drawLine(x + 4, y + 8, x + 7, y + 11, WHITE);
		break;

	case SCR_CLOCK_MENU_STOPWATCH:
		view_render.drawCircle(x, y + 1, 6, WHITE);
		view_render.drawRect(x - 2, y - 9, 5, 3, WHITE);
		view_render.drawLine(x, y + 1, x + 3, y - 3, WHITE);
		break;

	case SCR_CLOCK_MENU_TIMER:
		view_render.drawRect(x - 7, y - 5, 14, 12, WHITE);
		view_render.drawLine(x - 4, y - 8, x + 4, y - 8, WHITE);
		view_render.drawLine(x, y - 8, x, y - 5, WHITE);
		view_render.drawLine(x - 3, y + 1, x + 3, y + 1, WHITE);
		break;

	case SCR_CLOCK_MENU_SETTING:
		view_render.drawCircle(x, y, 7, WHITE);
		view_render.drawCircle(x, y, 2, WHITE);
		view_render.drawLine(x - 10, y, x - 7, y, WHITE);
		view_render.drawLine(x + 7, y, x + 10, y, WHITE);
		view_render.drawLine(x, y - 10, x, y - 7, WHITE);
		view_render.drawLine(x, y + 7, x, y + 10, WHITE);
		break;

	case SCR_CLOCK_MENU_EXIT:
	default:
		view_render.drawRect(x - 7, y - 7, 12, 14, WHITE);
		view_render.drawLine(x - 2, y, x + 8, y, WHITE);
		view_render.drawLine(x + 5, y - 3, x + 8, y, WHITE);
		view_render.drawLine(x + 5, y + 3, x + 8, y, WHITE);
		break;
	}
}

static const char* scr_clock_menu_item_label(uint8_t index)
{
	static const char* labels[] = {
	    "CLOCK",
	    "ALARM",
	    "SWATCH",
	    "TIMER",
	    "SET",
	    "EXIT",
	};

	if (index >= SCR_CLOCK_MENU_ITEM_NUMBER)
	{
		return "";
	}

	return labels[index];
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
