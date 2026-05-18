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
#define EEPROM_START_ADDR								(0X0000)
#define EEPROM_END_ADDR									(0X1000)
#define EEPROM_SCORE_START_ADDR         (0X0010)
#define EEPROM_SCORE_PLAY_ADDR          (0X00FA)
#define EEPROM_SETTING_START_ADDR       (0X0100)
#define ZW_GAME_EEPROM_MAGIC_NUMBER     ((uint32_t)0x170304CA)

/******************************************************************************/
/* Zomw*/
/******************************************************************************/

/* setting data */
#define ZW_GAME_SETTING_SILENT_OFF                   (0)
#define ZW_GAME_SETTING_SILENT_ON                    (1)
#define ZW_GAME_SETTING_NUM_CAR_DEFAULT              (0x15) // 0x15 => 10101 (Bin) => lane 0, 2 va 4 hien thi car
#define ZW_GAME_SETTING_ZOMBIE_SPEED_DEFAULT         (1)
#define ZW_GAME_SETTING_TOMBSTONE_LANE_1_DEFAULT     (0X15) // 0x15 => 10101 (Bin) => lane 0, 2 va 4 hien thi tombstone
#define ZW_GAME_SETTING_TOMBSTONE_LANE_2_DEFAULT     (0X04) // 0x15 => 00100 (Bin) => lane 2 hien thi tombstone => lane 2 co 2 tombstone

typedef struct {
  bool silent;
  uint8_t num_car;
  uint8_t zombie_speed;
  uint8_t tombstone_lane_1;
  uint8_t tombstone_lane_2;
} zw_game_setting_t;

/* score data */
typedef struct {
  uint16_t score_now;
  uint16_t score_1st;
  uint16_t score_2nd;
  uint16_t score_3rd;
} zw_game_score_t;

#ifdef __cplusplus
extern "C" {
#endif

extern zw_game_score_t gamescore;
extern bool zw_game_score_read(zw_game_score_t* data);
extern bool zw_game_score_write(zw_game_score_t* data);

extern zw_game_setting_t settingdata;
extern bool zw_game_setting_read(zw_game_setting_t* data);
extern bool zw_game_setting_write(zw_game_setting_t* data);

#ifdef __cplusplus
}
#endif

#endif //__APP_EEPROM_H__
