#include "app_eeprom.h"

#include "eeprom.h"

/*
 * Bo cuc EEPROM cho du lieu ben vung cua Zomwar.
 *
 * Moi ban ghi duoc luu theo dang:
 *   [magic_number][payload struct][8-bit additive checksum]
 *
 * magic_number danh dau ban ghi da khoi tao, tranh doc nham bytes EEPROM da
 * xoa hoac cua ung dung khac thanh du lieu hop le. checksum la tong byte cua
 * magic_number + payload; chi de phat hien hong nhe, khong phai bao mat.
 *
 * Bo cuc hien tai dung dia chi co dinh:
 *   EEPROM_SCORE_START_ADDR   -> zw_game_score_eeprom_t
 *   EEPROM_SETTING_START_ADDR -> zw_game_setting_eeprom_t
 *
 * Module nay khong co wear-leveling. Ben goi nen tranh ghi lap lai khi gia tri
 * khong thay doi.
 */

zw_game_setting_t settingdata;
zw_game_score_t   gamescore;

typedef struct {
	uint32_t magic_number;
	zw_game_setting_t data;
	uint8_t check_sum;
} zw_game_setting_eeprom_t;

typedef struct {
	uint32_t magic_number;
	zw_game_score_t data;
	uint8_t check_sum;
} zw_game_score_eeprom_t;

#define ZW_GAME_SETTING_CHECKSUM_SIZE	(sizeof(uint32_t) + sizeof(zw_game_setting_t))
#define ZW_GAME_SCORE_CHECKSUM_SIZE		(sizeof(uint32_t) + sizeof(zw_game_score_t))

static uint8_t zw_game_eeprom_checksum(uint8_t* data, uint32_t size) {
	uint8_t check_sum = 0;

	for (uint32_t i = 0; i < size; i++) {
		check_sum += data[i];
	}

	return check_sum;
}

static void zw_game_eeprom_update_checksum(uint32_t* magic_number, uint8_t* check_sum, uint32_t check_sum_size) {
	*magic_number = ZW_GAME_EEPROM_MAGIC_NUMBER;
	*check_sum    = zw_game_eeprom_checksum((uint8_t*)magic_number, check_sum_size);
}

static bool zw_game_eeprom_is_valid(uint32_t* magic_number, uint8_t check_sum, uint32_t check_sum_size) {
	return (*magic_number == ZW_GAME_EEPROM_MAGIC_NUMBER) &&
		   (check_sum == zw_game_eeprom_checksum((uint8_t*)magic_number, check_sum_size));
}

/*****************************************************************************/
/* Setting                                                                    */
/*****************************************************************************/
static void zw_game_setting_set_default(zw_game_setting_t* data) {
	data->silent           = ZW_GAME_SETTING_SILENT_OFF;
	data->num_car          = ZW_GAME_SETTING_NUM_CAR_DEFAULT;
	data->tombstone_lane_1 = ZW_GAME_SETTING_TOMBSTONE_LANE_1_DEFAULT;
	data->tombstone_lane_2 = ZW_GAME_SETTING_TOMBSTONE_LANE_2_DEFAULT;
	data->zombie_speed     = ZW_GAME_SETTING_ZOMBIE_SPEED_DEFAULT;
}

bool zw_game_setting_read(zw_game_setting_t* data) {
	zw_game_setting_eeprom_t eeprom_data;

	uint8_t ret = eeprom_read(EEPROM_SETTING_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data));

	if (ret == EEPROM_DRIVER_OK &&
		zw_game_eeprom_is_valid(&eeprom_data.magic_number, eeprom_data.check_sum, ZW_GAME_SETTING_CHECKSUM_SIZE)) {
		*data = eeprom_data.data;
		return true;
	}

	zw_game_setting_set_default(data);
	return false;
}

bool zw_game_setting_write(zw_game_setting_t* data) {
	zw_game_setting_eeprom_t eeprom_data;

	eeprom_data.data = *data;
	zw_game_eeprom_update_checksum(&eeprom_data.magic_number, &eeprom_data.check_sum, ZW_GAME_SETTING_CHECKSUM_SIZE);

	return eeprom_write(EEPROM_SETTING_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data)) == EEPROM_DRIVER_OK;
}

/*****************************************************************************/
/* Score                                                                      */
/*****************************************************************************/
static void zw_game_score_set_default(zw_game_score_t* data) {
	data->score_now = 0;
	data->score_1st = 0;
	data->score_2nd = 0;
	data->score_3rd = 0;
}

bool zw_game_score_read(zw_game_score_t* data) {
	zw_game_score_eeprom_t eeprom_data;

	uint8_t ret = eeprom_read(EEPROM_SCORE_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data));

	if (ret == EEPROM_DRIVER_OK &&
		zw_game_eeprom_is_valid(&eeprom_data.magic_number, eeprom_data.check_sum, ZW_GAME_SCORE_CHECKSUM_SIZE)) {
		*data = eeprom_data.data;
		return true;
	}

	zw_game_score_set_default(data);
	return false;
}

bool zw_game_score_write(zw_game_score_t* data) {
	zw_game_score_eeprom_t eeprom_data;

	eeprom_data.data = *data;
	zw_game_eeprom_update_checksum(&eeprom_data.magic_number, &eeprom_data.check_sum, ZW_GAME_SCORE_CHECKSUM_SIZE);

	return eeprom_write(EEPROM_SCORE_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data)) == EEPROM_DRIVER_OK;
}
