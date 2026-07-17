#include "scr_clock_world.h"

static void view_scr_clock_world();

view_dynamic_t dyn_view_scr_clock_world = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_clock_world};

view_screen_t scr_clock_world = {
    &dyn_view_scr_clock_world,
    ITEM_NULL,
    ITEM_NULL,
    .focus_item = 0,
};

static int16_t world_anim_offset = 0;

static int8_t world_anim_direction = 0;

static uint8_t world_anim_running = 0;

static uint8_t world_old_country = 0;

static void draw_world_clock_content(
    mc_world_clock_state_t* state,
    int16_t offset)
{
	const mc_world_clock_country_t* country =
	    &state->country[state->selected_country];

	char time_buf[12];

	xsprintf(time_buf,
	         "%02u:%02u:%02u",
	         state->hour,
	         state->minute,
	         state->second);

	char idx_buf[8];

	xsprintf(idx_buf,
	         "%u",
	         state->selected_country + 1);

	view_render.drawRect(
	    offset,
	    0,
	    128,
	    64,
	    WHITE);

	view_render.setTextSize(1);

	view_render.setCursor(offset + 30, 3);
	view_render.print("World Clock");

	view_render.drawLine(
	    offset,
	    12,
	    offset + 128,
	    12,
	    WHITE);

	view_render.setCursor(offset + 4, 16);
	view_render.print(country->name);

	view_render.setCursor(offset + 61, 16);
	view_render.print(country->utc_label);

	view_render.setCursor(offset + 116, 16);
	view_render.print(idx_buf);

	view_render.setTextSize(2);

	view_render.setCursor(offset + 20, 28);
	view_render.print(time_buf);

	view_render.setTextSize(1);

	view_render.setCursor(offset + 15, 52);
	view_render.print("UP");

	view_render.setCursor(offset + 50, 52);
	view_render.print("DOWN");

	view_render.setCursor(offset + 100, 52);
	view_render.print("MODE");
}

static void view_scr_clock_world()
{
	mc_world_clock_state_t state;

	mc_clock_world_clock_get_state(&state);

	view_render.clear();

	view_render.setTextColor(WHITE);

	if (world_anim_running)
	{
		mc_world_clock_state_t old_state = state;

		old_state.selected_country = world_old_country;

		/*
		 * old screen leaves
		 */
		draw_world_clock_content(
		    &old_state,
		    world_anim_offset);

		/*
		 * new screen enters
		 */
		draw_world_clock_content(
		    &state,
		    world_anim_offset + world_anim_direction * 128);

		/*
		 * smooth movement
		 */
		world_anim_offset -= world_anim_direction * 32;

		if (abs(world_anim_offset) >= 128)
		{
			world_anim_running = 0;
			world_anim_offset = 0;
		}
	}
	else
	{
		draw_world_clock_content(
		    &state,
		    0);
	}

	view_render.setTextColor(WHITE);
}

static void world_start_animation(int8_t direction)
{
	world_anim_direction = direction;

	world_anim_offset = 0;

	world_anim_running = 1;
}

void scr_clock_world_handle(
    ak_msg_t* msg)
{

	switch (msg->sig)
	{

	case SCREEN_ENTRY:
	{
		APP_DBG_SIG(
		    "SCREEN_ENTRY [world clock]\n");

		task_post_pure_msg(
		    MC_CLOCK_WORLD_CLOCK_ID,
		    MC_CLOCK_WORLD_CLOCK_SETUP);

		timer_set(
		    AC_TASK_DISPLAY_ID,
		    MC_CLOCK_TIME_TICK,
		    MC_CLOCK_TIME_TICK_INTERVAL - 950,
		    TIMER_PERIODIC);
	}
	break;

	case MC_CLOCK_TIME_TICK:
	{
		task_post_pure_msg(
		    MC_CLOCK_WORLD_CLOCK_ID,
		    MC_CLOCK_WORLD_CLOCK_UPDATE);
	}
	break;

	case AC_DISPLAY_BUTON_DOWN_PRESSED:
	{
		mc_world_clock_state_t state;

		mc_clock_world_clock_get_state(&state);

		world_old_country = state.selected_country;

		if (rand() & 1)
		{
			world_start_animation(-1);
		}
		else
		{
			world_start_animation(-1);
		}

		task_post_pure_msg(
		    MC_CLOCK_WORLD_CLOCK_ID,
		    MC_CLOCK_WORLD_CLOCK_SELECT_DOWN);

		BUZZER_PlaySound(
		    BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_UP_PRESSED:
	{

		mc_world_clock_state_t state;

		mc_clock_world_clock_get_state(&state);

		world_old_country =
		    state.selected_country;

		/*
		 * UP:
		 * random enter from top or left
		 */
		if (rand() & 1)
		{
			world_start_animation(1);
		}
		else
		{
			world_start_animation(1);
		}

		task_post_pure_msg(
		    MC_CLOCK_WORLD_CLOCK_ID,
		    MC_CLOCK_WORLD_CLOCK_SELECT_UP);

		BUZZER_PlaySound(
		    BUZZER_SOUND_CLICK);
	}
	break;

	case AC_DISPLAY_BUTON_MODE_PRESSED:
	{

		timer_remove_attr(
		    AC_TASK_DISPLAY_ID,
		    MC_CLOCK_TIME_TICK);

		BUZZER_PlaySound(
		    BUZZER_SOUND_CLICK);

		SCREEN_TRAN(
		    scr_clock_menu_handle,
		    &scr_clock_menu);
	}
	break;

	default:
		break;
	}
}