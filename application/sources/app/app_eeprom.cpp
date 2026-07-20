#include "app_eeprom.h"
#include "eeprom.h"

mc_clock_setting_t    clock_setting_data;
mc_clock_alarm_bank_t clock_alarm_data;

typedef struct
{
	uint32_t           magic_number;
	mc_clock_setting_t data;
	uint8_t            check_sum;
} mc_clock_setting_eeprom_t;

typedef struct
{
	uint32_t              magic_number;
	mc_clock_alarm_bank_t data;
	uint8_t               check_sum;
} mc_clock_alarm_eeprom_t;

#define MC_CLOCK_SETTING_CHECKSUM_SIZE (sizeof(uint32_t) + sizeof(mc_clock_setting_t))
#define MC_CLOCK_ALARM_CHECKSUM_SIZE   (sizeof(uint32_t) + sizeof(mc_clock_alarm_bank_t))

static uint8_t mc_clock_eeprom_checksum(uint8_t* data, uint32_t size)
{
	uint8_t check_sum = 0;

	for (uint32_t i = 0; i < size; i++)
	{
		check_sum += data[i];
	}

	return check_sum;
}

static void mc_clock_eeprom_update_checksum(uint32_t* magic_number, uint8_t* check_sum, uint32_t check_sum_size)
{
	*magic_number = MC_CLOCK_EEPROM_MAGIC_NUMBER;
	*check_sum    = mc_clock_eeprom_checksum((uint8_t*)magic_number, check_sum_size);
}

static bool mc_clock_eeprom_is_valid(uint32_t* magic_number, uint8_t check_sum, uint32_t check_sum_size)
{
	return (*magic_number == MC_CLOCK_EEPROM_MAGIC_NUMBER) &&
	       (check_sum == mc_clock_eeprom_checksum((uint8_t*)magic_number, check_sum_size));
}

/*****************************************************************************/
/* Setting                                                                    */
/*****************************************************************************/
static void mc_clock_setting_set_default(mc_clock_setting_t* data)
{
	data->format_12h    = MC_CLOCK_SETTING_FORMAT_12H_DEFAULT;
	data->sound_off     = MC_CLOCK_SETTING_SOUND_OFF_DEFAULT;
	data->chime_enabled = MC_CLOCK_SETTING_CHIME_DEFAULT;
	data->alarm_sound   = MC_CLOCK_SETTING_ALARM_SOUND_DEFAULT;
	data->world_country = MC_CLOCK_SETTING_WORLD_COUNTRY_DEFAULT;
}

bool mc_clock_setting_read(mc_clock_setting_t* data)
{
	mc_clock_setting_eeprom_t eeprom_data;

	uint8_t ret = eeprom_read(EEPROM_SETTING_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data));

	if (ret == EEPROM_DRIVER_OK &&
	    mc_clock_eeprom_is_valid(&eeprom_data.magic_number, eeprom_data.check_sum, MC_CLOCK_SETTING_CHECKSUM_SIZE))
	{
		*data = eeprom_data.data;
		return true;
	}

	mc_clock_setting_set_default(data);
	return false;
}

bool mc_clock_setting_write(mc_clock_setting_t* data)
{
	mc_clock_setting_eeprom_t eeprom_data;

	eeprom_data.data = *data;
	mc_clock_eeprom_update_checksum(&eeprom_data.magic_number, &eeprom_data.check_sum, MC_CLOCK_SETTING_CHECKSUM_SIZE);

	return eeprom_write(EEPROM_SETTING_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data)) == EEPROM_DRIVER_OK;
}

/*****************************************************************************/
/* Alarm bank                                                                 */
/*****************************************************************************/
static void mc_clock_alarm_bank_set_default(mc_clock_alarm_bank_t* data)
{
	for (uint8_t i = 0; i < MC_CLOCK_ALARM_MAX; i++)
	{
		data->alarm[i].hour    = 0;
		data->alarm[i].minute  = 0;
		data->alarm[i].enabled = 0;
	}
}

bool mc_clock_alarm_bank_read(mc_clock_alarm_bank_t* data)
{
	mc_clock_alarm_eeprom_t eeprom_data;

	uint8_t ret = eeprom_read(EEPROM_ALARM_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data));

	if (ret == EEPROM_DRIVER_OK &&
	    mc_clock_eeprom_is_valid(&eeprom_data.magic_number, eeprom_data.check_sum, MC_CLOCK_ALARM_CHECKSUM_SIZE))
	{
		*data = eeprom_data.data;
		return true;
	}

	mc_clock_alarm_bank_set_default(data);
	return false;
}

bool mc_clock_alarm_bank_write(mc_clock_alarm_bank_t* data)
{
	mc_clock_alarm_eeprom_t eeprom_data;

	eeprom_data.data = *data;
	mc_clock_eeprom_update_checksum(&eeprom_data.magic_number, &eeprom_data.check_sum, MC_CLOCK_ALARM_CHECKSUM_SIZE);

	return eeprom_write(EEPROM_ALARM_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data)) == EEPROM_DRIVER_OK;
}
