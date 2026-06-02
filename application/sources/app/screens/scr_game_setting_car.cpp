#include "scr_game_setting_car.h"

/*****************************************************************************/
/* Variable Declaration - Car position selection */
/*****************************************************************************/
static uint8_t car_pos_location_chosse;

/*****************************************************************************/
/* View - Car position selection */
/*****************************************************************************/
static void view_scr_game_setting_car();

view_dynamic_t dyn_view_game_setting_car = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_game_setting_car
};

view_screen_t scr_game_setting_car = {
	&dyn_view_game_setting_car,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

static void view_scr_game_setting_car() {
	view_render.setTextSize(1);

	uint8_t sel = car_pos_location_chosse;

	for (uint8_t i = 0; i <= CAR_POS_NUM_CARS; i++) {
		uint8_t frame_y  = CAR_POS_FRAMES_AXIS_Y_1 + CAR_POS_FRAMES_STEP * i;
		bool selected    = (i == sel);
		uint8_t fg       = selected ? BLACK : WHITE;

		if (selected) {
			view_render.fillRoundRect(CAR_POS_FRAMES_AXIS_X, frame_y,
									  CAR_POS_FRAMES_SIZE_W, CAR_POS_FRAMES_SIZE_H,
									  CAR_POS_FRAMES_SIZE_R, WHITE);
		} else {
			view_render.drawRoundRect(CAR_POS_FRAMES_AXIS_X, frame_y,
									  CAR_POS_FRAMES_SIZE_W, CAR_POS_FRAMES_SIZE_H,
									  CAR_POS_FRAMES_SIZE_R, WHITE);
		}

		view_render.setTextColor(fg);

		if (i < CAR_POS_NUM_CARS) {
			bool on = (settingdata.num_car >> i) & 1;
			view_render.setCursor(2, frame_y + 1);
			view_render.print("Car ");
			view_render.print(i + 1);
			view_render.setCursor(104, frame_y + 1);
			view_render.print("[");
			view_render.print(on ? "1" : "0");
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
/* Handle - Car position selection */
/*****************************************************************************/
void scr_game_setting_car_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		view_render.clear();
		car_pos_location_chosse = 0;
	}
		break;

	case AC_DISPLAY_BUTTON_MODE_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_PRESSED\n");
		if (car_pos_location_chosse < CAR_POS_NUM_CARS) {
			settingdata.num_car ^= (1 << car_pos_location_chosse);
			BUZZER_PlaySound(BUZZER_SOUND_CLICK);
		} else {
			zw_game_setting_write(&settingdata);
			SCREEN_TRAN(scr_game_setting_handle, &scr_game_setting);
			BUZZER_PlaySound(BUZZER_SOUND_STARTUP);
		}
	}
		break;

	case AC_DISPLAY_BUTTON_UP_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_PRESSED\n");
		if (car_pos_location_chosse == 0) {
			car_pos_location_chosse = CAR_POS_ITEM_EXIT;
		} else {
			car_pos_location_chosse--;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
		break;

	case AC_DISPLAY_BUTTON_DOWN_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_PRESSED\n");
		if (car_pos_location_chosse == CAR_POS_ITEM_EXIT) {
			car_pos_location_chosse = 0;
		} else {
			car_pos_location_chosse++;
		}
		BUZZER_PlaySound(BUZZER_SOUND_CLICK);
	}
		break;

	default:
		break;
	}
}
