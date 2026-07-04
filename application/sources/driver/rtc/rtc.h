#ifndef __RTC_H__
#define __RTC_H__

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>

#define RTC_WEEKDAY_MON (1)
#define RTC_WEEKDAY_SUN (7)
#define RTC_ALARM_ANY_WEEKDAY (0)

	typedef struct
	{
		uint8_t hour;
		uint8_t min;
		uint8_t sec;
	} rtc_time_t;

	typedef struct
	{
		uint8_t date;
		uint8_t month;
		uint16_t year;
		uint8_t weekday;
	} rtc_date_t;

	void rtc_init(void);

	void rtc_get_time(rtc_time_t* time);
	void rtc_set_time(const rtc_time_t* time);

	void rtc_get_date(rtc_date_t* date);
	void rtc_set_date(const rtc_date_t* date);

	void rtc_set_alarm_a(const rtc_time_t* time, uint8_t weekday);
	void rtc_clear_alarm_a(void);

	void rtc_set_wakeup(uint32_t seconds);
	void rtc_stop_wakeup(void);

	/* ISR entries wired into the vector table; never call directly. */
	void rtc_wakeup_irq(void);
	void rtc_alarm_irq(void);

#if defined(__cplusplus)
}
#endif

#endif /* __RTC_H__ */
