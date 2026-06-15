#ifndef __APP_EEPROM_H__
#define __APP_EEPROM_H__

#include <stdint.h>
#include "app.h"

/**
 *****************************************************************************
 * EEPROM define address.
 *
 *****************************************************************************
 */
#define EEPROM_START_ADDR (0X0000)
#define EEPROM_END_ADDR (0X1000)

#define EEPROM_SCORE_START_ADDR (0X0010)
#define EEPROM_SETTING_START_ADDR (0X0100)

#define ZW_GAME_EEPROM_MAGIC_NUMBER ((uint32_t)0x7A6F6D77) // "zomw"

/******************************************************************************/
/* Zomwar game                                                                */
/******************************************************************************/

/* setting data - default values & limits */
#define ZW_GAME_SETTING_SILENT_OFF (0)
#define ZW_GAME_SETTING_SILENT_ON (1)
#define ZW_GAME_SETTING_NUM_CAR_DEFAULT (0x1F)			// 0b11111 = all 5 cars enabled
#define ZW_GAME_SETTING_TOMBSTONE_LANE_1_DEFAULT (0x1F) // 0b11111 = all 5 tombstones in group 1
#define ZW_GAME_SETTING_TOMBSTONE_LANE_2_DEFAULT (0x00) // group 2 disabled by default
#define ZW_GAME_SETTING_ZOMBIE_SPEED_MIN (1)
#define ZW_GAME_SETTING_ZOMBIE_SPEED_MAX (5)
#define ZW_GAME_SETTING_ZOMBIE_SPEED_DEFAULT (2)

typedef struct
{
	bool silent;			  // mute/unmute sound
	uint8_t num_car;		  // bitmask 5 lanes: which lane has a car
	uint8_t tombstone_lane_1; // bitmask 5 lanes: tombstone group 1
	uint8_t tombstone_lane_2; // bitmask 5 lanes: tombstone group 2
	uint8_t zombie_speed;	  // 1..5
} zw_game_setting_t;

/* score data */
typedef struct
{
	uint32_t score_now; // last round score
	uint32_t score_1st; // 1st place score
	uint32_t score_2nd; // 2nd place score
	uint32_t score_3rd; // 3rd place score
} zw_game_score_t;

#ifdef __cplusplus
extern "C"
{
#endif

	extern zw_game_setting_t settingdata;
	extern bool zw_game_setting_read(zw_game_setting_t *data);
	extern bool zw_game_setting_write(zw_game_setting_t *data);

	extern zw_game_score_t gamescore;
	extern bool zw_game_score_read(zw_game_score_t *data);
	extern bool zw_game_score_write(zw_game_score_t *data);

#ifdef __cplusplus
}
#endif

#endif //__APP_EEPROM_H__
