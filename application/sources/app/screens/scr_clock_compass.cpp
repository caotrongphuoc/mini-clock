#include "scr_clock_compass.h"

static void view_scr_clock_compass();

view_dynamic_t dyn_view_scr_clock_compass =
    {
        {
            .item_type = ITEM_TYPE_DYNAMIC,
        },

        view_scr_clock_compass};

view_screen_t scr_clock_compass =
    {
        &dyn_view_scr_clock_compass,

        ITEM_NULL,
        ITEM_NULL,

        .focus_item = 0,
};

static void draw_compass_arrow(float heading)
{
	float angle =
	    heading * 3.14159265f / 180.0f;

	int16_t cx = 64;
	int16_t cy = 34;

	int16_t x =
	    cx + sin(angle) * 18;

	int16_t y =
	    cy - cos(angle) * 18;

	view_render.drawLine(
	    cx,
	    cy,
	    x,
	    y,
	    WHITE);

	view_render.fillCircle(
	    cx,
	    cy,
	    3,
	    WHITE);
}

static void draw_direction(float heading)
{
	const char* dir;

	if (heading < 22.5 ||
	    heading >= 337.5)
	{
		dir = "N";
	}
	else if (heading < 67.5)
	{
		dir = "NE";
	}
	else if (heading < 112.5)
	{
		dir = "E";
	}
	else if (heading < 157.5)
	{
		dir = "SE";
	}
	else if (heading < 202.5)
	{
		dir = "S";
	}
	else if (heading < 247.5)
	{
		dir = "SW";
	}
	else if (heading < 292.5)
	{
		dir = "W";
	}
	else
	{
		dir = "NW";
	}

	view_render.setTextSize(2);

	view_render.setCursor(
	    58,
	    5);

	view_render.print(dir);
}

static void view_scr_clock_compass()
{

	mc_clock_compass_state_t state;

	mc_clock_compass_get_state(
	    &state);

	view_render.clear();

	view_render.setTextSize(1);

	view_render.setCursor(
	    35,
	    0);

	view_render.print(
	    "COMPASS");

	/*
	   compass circle
	*/

	view_render.drawCircle(
	    64,
	    34,
	    22,
	    WHITE);

	/*
	   fixed directions
	*/

	view_render.setCursor(61, 10);
	view_render.print("N");

	view_render.setCursor(61, 50);
	view_render.print("S");

	view_render.setCursor(40, 32);
	view_render.print("W");

	view_render.setCursor(85, 32);
	view_render.print("E");

	/*
	   moving needle
	*/

	draw_compass_arrow(
	    state.heading);

	draw_direction(
	    state.heading);

	char buf[20];

	xsprintf(
	    buf,
	    "%03d deg",
	    (int)state.heading);

	view_render.setCursor(
	    45,
	    58);

	view_render.print(
	    buf);
}

void scr_clock_compass_handle(
    ak_msg_t* msg)
{

	switch (msg->sig)
	{

	case SCREEN_ENTRY:
	{

		APP_DBG_SIG(
		    "COMPASS ENTRY\n");

		task_post_pure_msg(
		    MC_CLOCK_COMPASS_ID,
		    MC_CLOCK_COMPASS_SETUP);

		timer_set(
		    AC_TASK_DISPLAY_ID,
		    MC_CLOCK_COMPASS_UPDATE,
		    100,
		    TIMER_PERIODIC);
	}
	break;

	case MC_CLOCK_COMPASS_UPDATE:
	{

		task_post_pure_msg(
		    MC_CLOCK_COMPASS_ID,
		    MC_CLOCK_COMPASS_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{

		timer_remove_attr(
		    AC_TASK_DISPLAY_ID,
		    MC_CLOCK_COMPASS_UPDATE);

		SCREEN_TRAN(
		    scr_clock_menu_handle,
		    &scr_clock_menu);
	}
	break;

	default:
		break;
	}
}