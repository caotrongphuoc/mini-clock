#ifndef __APP_EEPROM_H__
#define __APP_EEPROM_H__

#include <stdint.h>
#include <stdbool.h>
#include "app.h"

/**
 *****************************************************************************
 * EEPROM define address.
 *****************************************************************************
 */
#define EEPROM_START_ADDR         (0x0000)
#define EEPROM_END_ADDR           (0x1000)

#define EEPROM_SETTING_START_ADDR (0x0010)
#define EEPROM_ALARM_START_ADDR   (0x0100)

#define MC_CLOCK_EEPROM_MAGIC_NUMBER ((uint32_t)0x6D63636B) // "mcck"

/******************************************************************************/
/* Clock settings                                                             */
/******************************************************************************/

#define MC_CLOCK_SETTING_FORMAT_12H_DEFAULT    (0)
#define MC_CLOCK_SETTING_SOUND_OFF_DEFAULT     (0)
#define MC_CLOCK_SETTING_CHIME_DEFAULT         (0)
#define MC_CLOCK_SETTING_ALARM_SOUND_DEFAULT   (0)
#define MC_CLOCK_SETTING_WORLD_COUNTRY_DEFAULT (0)

typedef struct
{
	uint8_t format_12h;    // 0=24h, 1=12h
	uint8_t sound_off;     // 1=global mute
	uint8_t chime_enabled; // 1=hourly chime
	uint8_t alarm_sound;   // index into alarm_sound_list[]
	uint8_t world_country; // index into world clock country[]
} mc_clock_setting_t;

#ifdef __cplusplus
extern "C"
{
#endif

	extern mc_clock_setting_t clock_setting_data;
	extern bool mc_clock_setting_read(mc_clock_setting_t* data);
	extern bool mc_clock_setting_write(mc_clock_setting_t* data);

	extern uint8_t mc_clock_eeprom_checksum(uint8_t* data, uint32_t size);
	extern void    mc_clock_eeprom_update_checksum(uint32_t* magic_number, uint8_t* check_sum, uint32_t check_sum_size);
	extern bool    mc_clock_eeprom_is_valid(uint32_t* magic_number, uint8_t check_sum, uint32_t check_sum_size);

#ifdef __cplusplus
}
#endif

#endif //__APP_EEPROM_H__
