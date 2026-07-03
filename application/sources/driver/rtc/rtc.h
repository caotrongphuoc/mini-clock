#ifndef __RTC_H__
#define __RTC_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

/* Weekday convention: 1=Monday .. 7=Sunday (matches STM32 RTC). */
#define RTC_WEEKDAY_MON		(1)
#define RTC_WEEKDAY_SUN		(7)

/* Passed to rtc_set_alarm_a as weekday: match every day (date/weekday ignored). */
#define RTC_ALARM_ANY_WEEKDAY	(0)

typedef struct {
	uint8_t hour;	/* 0..23 (24h format) */
	uint8_t min;	/* 0..59 */
	uint8_t sec;	/* 0..59 */
} rtc_time_t;

typedef struct {
	uint8_t  date;		/* 1..31 */
	uint8_t  month;		/* 1..12 */
	uint16_t year;		/* full year e.g. 2026 */
	uint8_t  weekday;	/* 1..7, see RTC_WEEKDAY_* */
} rtc_date_t;

/* Initialize LSE + RTC calendar + 1s wakeup tick. Preserves time across
 * warm reset by checking a magic value in BKP register 0. */
void rtc_init(void);

void rtc_get_time(rtc_time_t* time);
void rtc_set_time(const rtc_time_t* time);

void rtc_get_date(rtc_date_t* date);
void rtc_set_date(const rtc_date_t* date);

/* Program Alarm A. weekday=RTC_ALARM_ANY_WEEKDAY fires daily; 1..7 fires only
 * when RTC weekday matches. Caller re-arms for other days as needed. */
void rtc_set_alarm_a(const rtc_time_t* time, uint8_t weekday);
void rtc_clear_alarm_a(void);

/* Wakeup timer (used for countdown + optional low-power tick). 0 disables. */
void rtc_set_wakeup(uint32_t seconds);
void rtc_stop_wakeup(void);

/* ISR entry points wired into the vector table (system.c). Each clears its
 * pending flag and posts a signal to MC_TASK_RTC_ID; never call directly. */
void rtc_wakeup_irq(void);
void rtc_alarm_irq(void);

#if defined(__cplusplus)
}
#endif

#endif /* __RTC_H__ */
