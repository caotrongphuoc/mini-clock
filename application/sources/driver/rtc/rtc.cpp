#include "rtc.h"

#include "Wire.h"

#define RTC_PCF85063_I2C_ADDR (0x51)
#define RTC_PCF85063_REG_CTRL_1 (0x00)
#define RTC_PCF85063_REG_CTRL_2 (0x01)
#define RTC_PCF85063_REG_SECONDS (0x04)
#define RTC_PCF85063_REG_MINUTES (0x05)
#define RTC_PCF85063_REG_HOURS (0x06)
#define RTC_PCF85063_REG_DAYS (0x07)
#define RTC_PCF85063_REG_WEEKDAYS (0x08)
#define RTC_PCF85063_REG_MONTHS (0x09)
#define RTC_PCF85063_REG_YEARS (0x0A)

#define RTC_PCF85063_SECONDS_OS (0x80)

static uint8_t rtc_bcd_to_bin(uint8_t value);
static uint8_t rtc_bin_to_bcd(uint8_t value);
static uint8_t rtc_weekday_to_pcf85063(uint8_t weekday);
static uint8_t rtc_weekday_from_pcf85063(uint8_t weekday);
static uint8_t rtc_read_regs(uint8_t reg, uint8_t* data, uint8_t len);
static uint8_t rtc_write_regs(uint8_t reg, const uint8_t* data, uint8_t len);
static void rtc_seed_default_time(void);

void rtc_init(void)
{
	Wire.begin();
	Wire.setClock(100000);

	uint8_t ctrl = 0;
	rtc_write_regs(RTC_PCF85063_REG_CTRL_1, &ctrl, 1);
	rtc_write_regs(RTC_PCF85063_REG_CTRL_2, &ctrl, 1);

	uint8_t seconds = 0;
	if (rtc_read_regs(RTC_PCF85063_REG_SECONDS, &seconds, 1) == 0)
	{
		return;
	}

	if (seconds & RTC_PCF85063_SECONDS_OS)
	{
		rtc_seed_default_time();
	}
}

void rtc_get_time(rtc_time_t* time)
{
	uint8_t data[3] = {0};

	if (rtc_read_regs(RTC_PCF85063_REG_SECONDS, data, sizeof(data)) == 0)
	{
		time->hour = 0;
		time->min = 0;
		time->sec = 0;
		return;
	}

	time->sec = rtc_bcd_to_bin(data[0] & 0x7F);
	time->min = rtc_bcd_to_bin(data[1] & 0x7F);
	time->hour = rtc_bcd_to_bin(data[2] & 0x3F);
}

void rtc_set_time(const rtc_time_t* time)
{
	uint8_t data[3];

	data[0] = rtc_bin_to_bcd(time->sec);
	data[1] = rtc_bin_to_bcd(time->min);
	data[2] = rtc_bin_to_bcd(time->hour);

	rtc_write_regs(RTC_PCF85063_REG_SECONDS, data, sizeof(data));
}

void rtc_get_date(rtc_date_t* date)
{
	uint8_t data[4] = {0};

	if (rtc_read_regs(RTC_PCF85063_REG_DAYS, data, sizeof(data)) == 0)
	{
		date->date = 1;
		date->month = 1;
		date->year = 2026;
		date->weekday = RTC_WEEKDAY_MON;
		return;
	}

	date->date = rtc_bcd_to_bin(data[0] & 0x3F);
	date->weekday = rtc_weekday_from_pcf85063(data[1] & 0x07);
	date->month = rtc_bcd_to_bin(data[2] & 0x1F);
	date->year = 2000 + rtc_bcd_to_bin(data[3]);
}

void rtc_set_date(const rtc_date_t* date)
{
	uint8_t data[4];

	data[0] = rtc_bin_to_bcd(date->date);
	data[1] = rtc_weekday_to_pcf85063(date->weekday);
	data[2] = rtc_bin_to_bcd(date->month);
	data[3] = rtc_bin_to_bcd((date->year >= 2000) ? (date->year - 2000) : 0);

	rtc_write_regs(RTC_PCF85063_REG_DAYS, data, sizeof(data));
}

void rtc_set_alarm_a(const rtc_time_t* time, uint8_t weekday)
{
	(void)time;
	(void)weekday;
}

void rtc_clear_alarm_a(void)
{
}

void rtc_set_wakeup(uint32_t seconds)
{
	(void)seconds;
}

void rtc_stop_wakeup(void)
{
}

void rtc_wakeup_irq(void)
{
}

void rtc_alarm_irq(void)
{
}

static uint8_t rtc_bcd_to_bin(uint8_t value)
{
	return ((value >> 4) * 10) + (value & 0x0F);
}

static uint8_t rtc_bin_to_bcd(uint8_t value)
{
	return ((value / 10) << 4) | (value % 10);
}

static uint8_t rtc_weekday_to_pcf85063(uint8_t weekday)
{
	if (weekday == RTC_WEEKDAY_SUN)
	{
		return 0;
	}
	return weekday;
}

static uint8_t rtc_weekday_from_pcf85063(uint8_t weekday)
{
	if (weekday == 0)
	{
		return RTC_WEEKDAY_SUN;
	}
	return weekday;
}

static uint8_t rtc_read_regs(uint8_t reg, uint8_t* data, uint8_t len)
{
	if (len == 0)
	{
		return 1;
	}

	Wire.beginTransmission(RTC_PCF85063_I2C_ADDR);
	Wire.write(reg);
	if (Wire.endTransmission(false) != 0)
	{
		return 0;
	}

	if (Wire.requestFrom((uint8_t)RTC_PCF85063_I2C_ADDR, len) != len)
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

static uint8_t rtc_write_regs(uint8_t reg, const uint8_t* data, uint8_t len)
{
	Wire.beginTransmission(RTC_PCF85063_I2C_ADDR);
	Wire.write(reg);
	for (uint8_t i = 0; i < len; i++)
	{
		Wire.write(data[i]);
	}

	return Wire.endTransmission() == 0;
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
