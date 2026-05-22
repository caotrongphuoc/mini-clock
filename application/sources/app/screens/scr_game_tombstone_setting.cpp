#include "scr_game_tombstone_setting.h"

/*****************************************************************************/
/* Variable Declaration - Tombstone count per lane */
/*****************************************************************************/
static uint8_t tb_count_location_chosse;

/*****************************************************************************/
/* View - Tombstone count per lane */
/*****************************************************************************/
static void view_scr_game_tombstone_setting();

view_dynamic_t dyn_view_game_tombstone_setting = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_tombstone_setting
};

view_screen_t scr_game_tombstone_setting = {
	&dyn_view_game_tombstone_setting,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

/* Lay so bia mo hien tai cua lane i (0, 1, hoac 2) */
static uint8_t get_lane_count(uint8_t i) {
	uint8_t c = 0;
	if ((settingdata.tombstone_lane_1 >> i) & 1) c++;
	if ((settingdata.tombstone_lane_2 >> i) & 1) c++;
	return c;
}

static void view_scr_game_tombstone_setting() {
	view_render.setTextSize(1);

	uint8_t sel = (tb_count_location_chosse / TB_COUNT_STEP_CHOSSE) - 1;

	for (uint8_t i = 0; i <= TB_COUNT_NUM_LANES; i++) {
		uint8_t frame_y = TB_COUNT_FRAMES_AXIS_Y_1 + TB_COUNT_FRAMES_STEP * i;
		bool selected   = (i == sel);
		uint8_t fg      = selected ? BLACK : WHITE;

		if (selected) {
			view_render.fillRoundRect(TB_COUNT_FRAMES_AXIS_X, frame_y,
									  TB_COUNT_FRAMES_SIZE_W, TB_COUNT_FRAMES_SIZE_H,
									  TB_COUNT_FRAMES_SIZE_R, WHITE);
		} else {
			view_render.drawRoundRect(TB_COUNT_FRAMES_AXIS_X, frame_y,
									  TB_COUNT_FRAMES_SIZE_W, TB_COUNT_FRAMES_SIZE_H,
									  TB_COUNT_FRAMES_SIZE_R, WHITE);
		}

		view_render.setTextColor(fg);

		if (i < TB_COUNT_NUM_LANES) {
			view_render.setCursor(2, frame_y + 1);
			view_render.print("Lane ");
			view_render.print(i + 1);
			view_render.setCursor(104, frame_y + 1);
			view_render.print("[");
			view_render.print(get_lane_count(i));
			view_render.print("]");
		} else {
			view_render.setCursor(45, frame_y + 1);
			view_render.print(" EXIT ");
		}
	}

	view_render.setTextColor(WHITE);
	view_render.update();
}

/*****************************************************************************/
/* Handle - Tombstone count per lane */
/*****************************************************************************/
void scr_game_tombstone_setting_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		view_render.clear();
		tb_count_location_chosse = TB_COUNT_ITEM_ARRDESS_1;
	}
		break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		switch (tb_count_location_chosse) {
		case TB_COUNT_ITEM_ARRDESS_1:
		case TB_COUNT_ITEM_ARRDESS_2:
		case TB_COUNT_ITEM_ARRDESS_3:
		case TB_COUNT_ITEM_ARRDESS_4:
		case TB_COUNT_ITEM_ARRDESS_5: {
			uint8_t idx = (tb_count_location_chosse / TB_COUNT_STEP_CHOSSE) - 1;
			/* Cycle: 0 -> 1 -> 2 -> 0 */
			uint8_t cur = get_lane_count(idx);
			cur = (cur + 1) % 3;
			/* Clear ca 2 bit cua lane nay */
			settingdata.tombstone_lane_1 &= ~(1 << idx);
			settingdata.tombstone_lane_2 &= ~(1 << idx);
			/* Set theo gia tri moi */
			if (cur >= 1) settingdata.tombstone_lane_1 |= (1 << idx);
			if (cur == 2) settingdata.tombstone_lane_2 |= (1 << idx);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		}
			break;

		case TB_COUNT_ITEM_ARRDESS_6: {
			/* Luu EEPROM va ve settings */
			zw_game_setting_write(&settingdata);
			SCREEN_TRAN(scr_game_setting_handle, &scr_game_setting);
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
		}
			break;

		default:
			break;
		}
	}
		break;

	case AC_DISPLAY_BUTTON_UP_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED\n");
		tb_count_location_chosse -= TB_COUNT_STEP_CHOSSE;
		if (tb_count_location_chosse == TB_COUNT_ITEM_ARRDESS_0) {
			tb_count_location_chosse = TB_COUNT_ITEM_ARRDESS_6;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
		break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED\n");
		tb_count_location_chosse += TB_COUNT_STEP_CHOSSE;
		if (tb_count_location_chosse > TB_COUNT_ITEM_ARRDESS_6) {
			tb_count_location_chosse = TB_COUNT_ITEM_ARRDESS_1;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
		break;

	default:
		break;
	}
}
