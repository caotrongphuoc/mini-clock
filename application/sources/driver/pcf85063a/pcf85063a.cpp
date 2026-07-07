/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Jaakko Salo
 *
 * Portions adapted from https://github.com/jvsalo/pcf85063a for this project.
 */

#include "pcf85063a.h"

#include "Wire.h"

#define PCF85063A_I2C_ADDR (0x51)

#define PCF85063A_REG_CTRL_1 (0x00)
#define PCF85063A_REG_CTRL_2 (0x01)
#define PCF85063A_REG_SECONDS (0x04)
#define PCF85063A_REG_DAYS (0x07)
#define PCF85063A_REG_ALARM (0x0B)
#define PCF85063A_REG_TIMER_VALUE (0x10)
#define PCF85063A_REG_TIMER_MODE (0x11)

#define PCF85063A_SECONDS_OS (0x80)
#define PCF85063A_ALARM_DISABLE (0x80)

#define PCF85063A_CTRL_2_AIE (0x80)
#define PCF85063A_CTRL_2_AF (0x40)

#define PCF85063A_TIMER_MODE_TE (0x04)
#define PCF85063A_TIMER_MODE_TIE (0x02)
#define PCF85063A_TIMER_MODE_1HZ (0x10)

static uint8_t pcf85063a_bcd_to_bin(uint8_t value);
static uint8_t pcf85063a_bin_to_bcd(uint8_t value);
static uint8_t pcf85063a_weekday_to_reg(uint8_t weekday);
static uint8_t pcf85063a_weekday_from_reg(uint8_t weekday);
static uint8_t pcf85063a_read_regs(uint8_t reg, uint8_t* data, uint8_t len);
static uint8_t pcf85063a_write_regs(uint8_t reg, const uint8_t* data, uint8_t len);

void pcf85063a_init(void)
{
	Wire.begin();
	Wire.setClock(100000);

	uint8_t ctrl = 0;
	pcf85063a_write_regs(PCF85063A_REG_CTRL_1, &ctrl, 1);
	pcf85063a_write_regs(PCF85063A_REG_CTRL_2, &ctrl, 1);
}

uint8_t pcf85063a_is_time_lost(void)
{
	uint8_t seconds = 0;

	if (pcf85063a_read_regs(PCF85063A_REG_SECONDS, &seconds, 1) == 0)
	{
		return 1;
	}

	return (seconds & PCF85063A_SECONDS_OS) ? 1 : 0;
}

uint8_t pcf85063a_get_time(rtc_time_t* time)
{
	uint8_t data[3] = {0};

	if (pcf85063a_read_regs(PCF85063A_REG_SECONDS, data, sizeof(data)) == 0)
	{
		return 0;
	}

	time->sec = pcf85063a_bcd_to_bin(data[0] & 0x7F);
	time->min = pcf85063a_bcd_to_bin(data[1] & 0x7F);
	time->hour = pcf85063a_bcd_to_bin(data[2] & 0x3F);

	return 1;
}

uint8_t pcf85063a_set_time(const rtc_time_t* time)
{
	uint8_t data[3];

	data[0] = pcf85063a_bin_to_bcd(time->sec);
	data[1] = pcf85063a_bin_to_bcd(time->min);
	data[2] = pcf85063a_bin_to_bcd(time->hour);

	return pcf85063a_write_regs(PCF85063A_REG_SECONDS, data, sizeof(data));
}

uint8_t pcf85063a_get_date(rtc_date_t* date)
{
	uint8_t data[4] = {0};

	if (pcf85063a_read_regs(PCF85063A_REG_DAYS, data, sizeof(data)) == 0)
	{
		return 0;
	}

	date->date = pcf85063a_bcd_to_bin(data[0] & 0x3F);
	date->weekday = pcf85063a_weekday_from_reg(data[1] & 0x07);
	date->month = pcf85063a_bcd_to_bin(data[2] & 0x1F);
	date->year = 2000 + pcf85063a_bcd_to_bin(data[3]);

	return 1;
}

uint8_t pcf85063a_set_date(const rtc_date_t* date)
{
	uint8_t data[4];

	data[0] = pcf85063a_bin_to_bcd(date->date);
	data[1] = pcf85063a_weekday_to_reg(date->weekday);
	data[2] = pcf85063a_bin_to_bcd(date->month);
	data[3] = pcf85063a_bin_to_bcd((date->year >= 2000) ? (date->year - 2000) : 0);

	return pcf85063a_write_regs(PCF85063A_REG_DAYS, data, sizeof(data));
}

uint8_t pcf85063a_set_alarm(const rtc_time_t* time, uint8_t weekday)
{
	uint8_t data[5];
	uint8_t ctrl_2 = 0;

	data[0] = pcf85063a_bin_to_bcd(time->sec);
	data[1] = pcf85063a_bin_to_bcd(time->min);
	data[2] = pcf85063a_bin_to_bcd(time->hour);
	data[3] = PCF85063A_ALARM_DISABLE;
	data[4] = (weekday == RTC_ALARM_ANY_WEEKDAY) ?
	    PCF85063A_ALARM_DISABLE :
	    pcf85063a_weekday_to_reg(weekday);

	if (pcf85063a_write_regs(PCF85063A_REG_ALARM, data, sizeof(data)) == 0)
	{
		return 0;
	}

	if (pcf85063a_read_regs(PCF85063A_REG_CTRL_2, &ctrl_2, 1) == 0)
	{
		return 0;
	}

	ctrl_2 &= ~PCF85063A_CTRL_2_AF;
	ctrl_2 |= PCF85063A_CTRL_2_AIE;

	return pcf85063a_write_regs(PCF85063A_REG_CTRL_2, &ctrl_2, 1);
}

uint8_t pcf85063a_clear_alarm(void)
{
	uint8_t ctrl_2 = 0;

	if (pcf85063a_read_regs(PCF85063A_REG_CTRL_2, &ctrl_2, 1) == 0)
	{
		return 0;
	}

	ctrl_2 &= ~(PCF85063A_CTRL_2_AIE | PCF85063A_CTRL_2_AF);

	return pcf85063a_write_regs(PCF85063A_REG_CTRL_2, &ctrl_2, 1);
}

uint8_t pcf85063a_set_countdown(uint32_t seconds)
{
	uint8_t timer_mode = 0;
	uint8_t timer_value;

	if (seconds == 0)
	{
		return pcf85063a_stop_countdown();
	}

	if (seconds > 255)
	{
		seconds = 255;
	}

	timer_value = (uint8_t)seconds;

	if (pcf85063a_write_regs(PCF85063A_REG_TIMER_MODE, &timer_mode, 1) == 0)
	{
		return 0;
	}

	timer_mode = PCF85063A_TIMER_MODE_TE |
	             PCF85063A_TIMER_MODE_TIE |
	             PCF85063A_TIMER_MODE_1HZ;

	if (pcf85063a_write_regs(PCF85063A_REG_TIMER_VALUE, &timer_value, 1) == 0)
	{
		return 0;
	}

	return pcf85063a_write_regs(PCF85063A_REG_TIMER_MODE, &timer_mode, 1);
}

uint8_t pcf85063a_stop_countdown(void)
{
	uint8_t timer_mode = 0;

	return pcf85063a_write_regs(PCF85063A_REG_TIMER_MODE, &timer_mode, 1);
}

static uint8_t pcf85063a_bcd_to_bin(uint8_t value)
{
	return ((value >> 4) * 10) + (value & 0x0F);
}

static uint8_t pcf85063a_bin_to_bcd(uint8_t value)
{
	return ((value / 10) << 4) | (value % 10);
}

static uint8_t pcf85063a_weekday_to_reg(uint8_t weekday)
{
	if (weekday == RTC_WEEKDAY_SUN)
	{
		return 0;
	}

	return weekday;
}

static uint8_t pcf85063a_weekday_from_reg(uint8_t weekday)
{
	if (weekday == 0)
	{
		return RTC_WEEKDAY_SUN;
	}

	return weekday;
}

static uint8_t pcf85063a_read_regs(uint8_t reg, uint8_t* data, uint8_t len)
{
	if (len == 0)
	{
		return 1;
	}

	Wire.beginTransmission(PCF85063A_I2C_ADDR);
	Wire.write(reg);
	if (Wire.endTransmission(false) != 0)
	{
		return 0;
	}

	if (Wire.requestFrom((uint8_t)PCF85063A_I2C_ADDR, len) != len)
	{
		return 0;
	}

	for (uint8_t i = 0; i < len; i++)
	{
		int value = Wire.read();
		if (value < 0)
		{
			return 0;
		}
		data[i] = (uint8_t)value;
	}

	return 1;
}

static uint8_t pcf85063a_write_regs(uint8_t reg, const uint8_t* data, uint8_t len)
{
	Wire.beginTransmission(PCF85063A_I2C_ADDR);
	Wire.write(reg);
	for (uint8_t i = 0; i < len; i++)
	{
		Wire.write(data[i]);
	}

	return Wire.endTransmission() == 0;
}
