#include "rtc.h"

#include "pcf85063a.h"

static void rtc_seed_default_time(void);

void rtc_init(void)
{
	pcf85063a_init();

	if (pcf85063a_is_time_lost())
	{
		rtc_seed_default_time();
	}
}

void rtc_get_time(rtc_time_t* time)
{
	if (pcf85063a_get_time(time) == 0)
	{
		time->hour = 0;
		time->min = 0;
		time->sec = 0;
	}
}

void rtc_set_time(const rtc_time_t* time)
{
	pcf85063a_set_time(time);
}

void rtc_get_date(rtc_date_t* date)
{
	if (pcf85063a_get_date(date) == 0)
	{
		date->date = 1;
		date->month = 1;
		date->year = 2026;
		date->weekday = RTC_WEEKDAY_MON;
	}
}

void rtc_set_date(const rtc_date_t* date)
{
	pcf85063a_set_date(date);
}

void rtc_set_alarm_a(const rtc_time_t* time, uint8_t weekday)
{
	pcf85063a_set_alarm(time, weekday);
}

void rtc_clear_alarm_a(void)
{
	pcf85063a_clear_alarm();
}

void rtc_set_wakeup(uint32_t seconds)
{
	pcf85063a_set_countdown(seconds);
}

void rtc_stop_wakeup(void)
{
	pcf85063a_stop_countdown();
}

void rtc_wakeup_irq(void)
{
}

void rtc_alarm_irq(void)
{
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
