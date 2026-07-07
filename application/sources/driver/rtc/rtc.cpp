/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Jaakko Salo
 *
 * Portions adapted from https://github.com/jvsalo/pcf85063a for this project.
 */

#include "rtc.h"

#include "Wire.h"

#define RTC_PCF85063A_I2C_ADDR (0x51)

#define RTC_PCF85063A_REG_CTRL_1 (0x00)
#define RTC_PCF85063A_REG_CTRL_2 (0x01)
#define RTC_PCF85063A_REG_SECONDS (0x04)
#define RTC_PCF85063A_REG_DAYS (0x07)
#define RTC_PCF85063A_REG_ALARM (0x0B)
#define RTC_PCF85063A_REG_TIMER_VALUE (0x10)
#define RTC_PCF85063A_REG_TIMER_MODE (0x11)

#define RTC_PCF85063A_SECONDS_OS (0x80)
#define RTC_PCF85063A_ALARM_DISABLE (0x80)

#define RTC_PCF85063A_CTRL_2_AIE (0x80)
#define RTC_PCF85063A_CTRL_2_AF (0x40)

#define RTC_PCF85063A_TIMER_MODE_TE (0x04)
#define RTC_PCF85063A_TIMER_MODE_TIE (0x02)
#define RTC_PCF85063A_TIMER_MODE_1HZ (0x10)

static uint8_t rtc_pcf85063a_bcd_to_bin(uint8_t value);
static uint8_t rtc_pcf85063a_bin_to_bcd(uint8_t value);
static uint8_t rtc_pcf85063a_weekday_to_reg(uint8_t weekday);
static uint8_t rtc_pcf85063a_weekday_from_reg(uint8_t weekday);
static uint8_t rtc_pcf85063a_read_regs(uint8_t reg, uint8_t* data, uint8_t len);
static uint8_t rtc_pcf85063a_write_regs(uint8_t reg, const uint8_t* data, uint8_t len);
static uint8_t rtc_pcf85063a_is_time_lost(void);
static void rtc_seed_default_time(void);

void rtc_init(void)
{
	Wire.begin();
	Wire.setClock(100000);

	uint8_t ctrl = 0;
	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_CTRL_1, &ctrl, 1);
	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_CTRL_2, &ctrl, 1);

	if (rtc_pcf85063a_is_time_lost())
	{
		rtc_seed_default_time();
	}
}

void rtc_get_time(rtc_time_t* time)
{
	uint8_t data[3] = {0};

	if (rtc_pcf85063a_read_regs(RTC_PCF85063A_REG_SECONDS, data, sizeof(data)) == 0)
	{
		time->hour = 0;
		time->min = 0;
		time->sec = 0;
		return;
	}

	time->sec = rtc_pcf85063a_bcd_to_bin(data[0] & 0x7F);
	time->min = rtc_pcf85063a_bcd_to_bin(data[1] & 0x7F);
	time->hour = rtc_pcf85063a_bcd_to_bin(data[2] & 0x3F);
}

void rtc_set_time(const rtc_time_t* time)
{
	uint8_t data[3];

	data[0] = rtc_pcf85063a_bin_to_bcd(time->sec);
	data[1] = rtc_pcf85063a_bin_to_bcd(time->min);
	data[2] = rtc_pcf85063a_bin_to_bcd(time->hour);

	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_SECONDS, data, sizeof(data));
}

void rtc_get_date(rtc_date_t* date)
{
	uint8_t data[4] = {0};

	if (rtc_pcf85063a_read_regs(RTC_PCF85063A_REG_DAYS, data, sizeof(data)) == 0)
	{
		date->date = 1;
		date->month = 1;
		date->year = 2026;
		date->weekday = RTC_WEEKDAY_MON;
		return;
	}

	date->date = rtc_pcf85063a_bcd_to_bin(data[0] & 0x3F);
	date->weekday = rtc_pcf85063a_weekday_from_reg(data[1] & 0x07);
	date->month = rtc_pcf85063a_bcd_to_bin(data[2] & 0x1F);
	date->year = 2000 + rtc_pcf85063a_bcd_to_bin(data[3]);
}

void rtc_set_date(const rtc_date_t* date)
{
	uint8_t data[4];

	data[0] = rtc_pcf85063a_bin_to_bcd(date->date);
	data[1] = rtc_pcf85063a_weekday_to_reg(date->weekday);
	data[2] = rtc_pcf85063a_bin_to_bcd(date->month);
	data[3] = rtc_pcf85063a_bin_to_bcd((date->year >= 2000) ? (date->year - 2000) : 0);

	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_DAYS, data, sizeof(data));
}

void rtc_set_alarm_a(const rtc_time_t* time, uint8_t weekday)
{
	uint8_t data[5];
	uint8_t ctrl_2 = 0;

	data[0] = rtc_pcf85063a_bin_to_bcd(time->sec);
	data[1] = rtc_pcf85063a_bin_to_bcd(time->min);
	data[2] = rtc_pcf85063a_bin_to_bcd(time->hour);
	data[3] = RTC_PCF85063A_ALARM_DISABLE;
	data[4] = (weekday == RTC_ALARM_ANY_WEEKDAY) ?
	    RTC_PCF85063A_ALARM_DISABLE :
	    rtc_pcf85063a_weekday_to_reg(weekday);

	if (rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_ALARM, data, sizeof(data)) == 0)
	{
		return;
	}

	if (rtc_pcf85063a_read_regs(RTC_PCF85063A_REG_CTRL_2, &ctrl_2, 1) == 0)
	{
		return;
	}

	ctrl_2 &= ~RTC_PCF85063A_CTRL_2_AF;
	ctrl_2 |= RTC_PCF85063A_CTRL_2_AIE;
	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_CTRL_2, &ctrl_2, 1);
}

void rtc_clear_alarm_a(void)
{
	uint8_t ctrl_2 = 0;

	if (rtc_pcf85063a_read_regs(RTC_PCF85063A_REG_CTRL_2, &ctrl_2, 1) == 0)
	{
		return;
	}

	ctrl_2 &= ~(RTC_PCF85063A_CTRL_2_AIE | RTC_PCF85063A_CTRL_2_AF);
	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_CTRL_2, &ctrl_2, 1);
}

void rtc_set_wakeup(uint32_t seconds)
{
	uint8_t timer_mode = 0;
	uint8_t timer_value;

	if (seconds == 0)
	{
		rtc_stop_wakeup();
		return;
	}

	if (seconds > 255)
	{
		seconds = 255;
	}

	timer_value = (uint8_t)seconds;

	if (rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_TIMER_MODE, &timer_mode, 1) == 0)
	{
		return;
	}

	timer_mode = RTC_PCF85063A_TIMER_MODE_TE |
	             RTC_PCF85063A_TIMER_MODE_TIE |
	             RTC_PCF85063A_TIMER_MODE_1HZ;

	if (rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_TIMER_VALUE, &timer_value, 1) == 0)
	{
		return;
	}

	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_TIMER_MODE, &timer_mode, 1);
}

void rtc_stop_wakeup(void)
{
	uint8_t timer_mode = 0;

	rtc_pcf85063a_write_regs(RTC_PCF85063A_REG_TIMER_MODE, &timer_mode, 1);
}

void rtc_wakeup_irq(void)
{
}

void rtc_alarm_irq(void)
{
}

static uint8_t rtc_pcf85063a_bcd_to_bin(uint8_t value)
{
	return ((value >> 4) * 10) + (value & 0x0F);
}

static uint8_t rtc_pcf85063a_bin_to_bcd(uint8_t value)
{
	return ((value / 10) << 4) | (value % 10);
}

static uint8_t rtc_pcf85063a_weekday_to_reg(uint8_t weekday)
{
	if (weekday == RTC_WEEKDAY_SUN)
	{
		return 0;
	}

	return weekday;
}

static uint8_t rtc_pcf85063a_weekday_from_reg(uint8_t weekday)
{
	if (weekday == 0)
	{
		return RTC_WEEKDAY_SUN;
	}

	return weekday;
}

static uint8_t rtc_pcf85063a_read_regs(uint8_t reg, uint8_t* data, uint8_t len)
{
	if (len == 0)
	{
		return 1;
	}

	Wire.beginTransmission(RTC_PCF85063A_I2C_ADDR);
	Wire.write(reg);
	if (Wire.endTransmission(false) != 0)
	{
		return 0;
	}

	if (Wire.requestFrom((uint8_t)RTC_PCF85063A_I2C_ADDR, len) != len)
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

static uint8_t rtc_pcf85063a_write_regs(uint8_t reg, const uint8_t* data, uint8_t len)
{
	Wire.beginTransmission(RTC_PCF85063A_I2C_ADDR);
	Wire.write(reg);
	for (uint8_t i = 0; i < len; i++)
	{
		Wire.write(data[i]);
	}

	return Wire.endTransmission() == 0;
}

static uint8_t rtc_pcf85063a_is_time_lost(void)
{
	uint8_t seconds = 0;

	if (rtc_pcf85063a_read_regs(RTC_PCF85063A_REG_SECONDS, &seconds, 1) == 0)
	{
		return 1;
	}

	return (seconds & RTC_PCF85063A_SECONDS_OS) ? 1 : 0;
}

static void rtc_seed_default_time(void)
{
	rtc_date_t date;
	date.date = 1;
	date.month = 1;
	date.year = 2026;
	date.weekday = 4;
	rtc_set_date(&date);

	rtc_time_t time;
	time.hour = 0;
	time.min = 0;
	time.sec = 0;
	rtc_set_time(&time);
}
