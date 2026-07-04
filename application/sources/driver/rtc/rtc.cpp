#include "rtc.h"

#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_pwr.h"
#include "stm32l1xx_rtc.h"
#include "stm32l1xx_exti.h"
#include "misc.h"

/* ak.h must precede message.h — ak.h pulls in fsm.h/task.h which reference
 * ak_msg_t, defined in message.h; including message.h first re-enters it
 * before its own typedef is visible. */
#include "ak.h"
#include "message.h"
#include "task_list.h"
#include "app.h"

#define RTC_BKP_MAGIC			(0xC1CCC10CUL)
#define RTC_LSE_STARTUP_TIMEOUT	(2000000UL)

static void rtc_config_exti_and_nvic(void);
static ErrorStatus rtc_calendar_init(void);
static void rtc_seed_default_time(void);

void rtc_init(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_RTCAccessCmd(ENABLE);

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != RTC_BKP_MAGIC) {
		RCC_LSEConfig(RCC_LSE_ON);
		uint32_t timeout = RTC_LSE_STARTUP_TIMEOUT;
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && --timeout) {
		}

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();

		if (rtc_calendar_init() == SUCCESS) {
			rtc_seed_default_time();
			RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_BKP_MAGIC);
		}
	}
	else {
		RTC_WaitForSynchro();
	}

	/* 32768 / (128 * 256) = 1 Hz → wakeup counter = 0 → tick every 1s. */
	RTC_WakeUpCmd(DISABLE);
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	RTC_SetWakeUpCounter(0);
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line20);
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	RTC_WakeUpCmd(ENABLE);

	rtc_config_exti_and_nvic();
}

void rtc_get_time(rtc_time_t* time) {
	RTC_TimeTypeDef t;
	RTC_GetTime(RTC_Format_BIN, &t);
	time->hour = t.RTC_Hours;
	time->min  = t.RTC_Minutes;
	time->sec  = t.RTC_Seconds;
}

void rtc_set_time(const rtc_time_t* time) {
	RTC_TimeTypeDef t;
	t.RTC_H12     = RTC_H12_AM;
	t.RTC_Hours   = time->hour;
	t.RTC_Minutes = time->min;
	t.RTC_Seconds = time->sec;
	RTC_SetTime(RTC_Format_BIN, &t);
	RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_BKP_MAGIC);
}

void rtc_get_date(rtc_date_t* date) {
	RTC_DateTypeDef d;
	RTC_GetDate(RTC_Format_BIN, &d);
	date->date    = d.RTC_Date;
	date->month   = d.RTC_Month;
	date->year    = 2000 + d.RTC_Year;
	date->weekday = d.RTC_WeekDay;
}

void rtc_set_date(const rtc_date_t* date) {
	RTC_DateTypeDef d;
	d.RTC_Date    = date->date;
	d.RTC_Month   = date->month;
	d.RTC_Year    = (date->year >= 2000) ? (date->year - 2000) : 0;
	d.RTC_WeekDay = date->weekday;
	RTC_SetDate(RTC_Format_BIN, &d);
	RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_BKP_MAGIC);
}

void rtc_set_alarm_a(const rtc_time_t* time, uint8_t weekday) {
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	RTC_AlarmTypeDef a;
	a.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	a.RTC_AlarmTime.RTC_Hours   = time->hour;
	a.RTC_AlarmTime.RTC_Minutes = time->min;
	a.RTC_AlarmTime.RTC_Seconds = time->sec;

	if (weekday == RTC_ALARM_ANY_WEEKDAY) {
		a.RTC_AlarmMask           = RTC_AlarmMask_DateWeekDay;
		a.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
		a.RTC_AlarmDateWeekDay    = RTC_WEEKDAY_MON;
	}
	else {
		a.RTC_AlarmMask           = RTC_AlarmMask_None;
		a.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
		a.RTC_AlarmDateWeekDay    = weekday;
	}

	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &a);
	RTC_ClearITPendingBit(RTC_IT_ALRA);
	EXTI_ClearITPendingBit(EXTI_Line17);
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}

void rtc_clear_alarm_a(void) {
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	RTC_ClearITPendingBit(RTC_IT_ALRA);
	EXTI_ClearITPendingBit(EXTI_Line17);
}

void rtc_set_wakeup(uint32_t seconds) {
	if (seconds == 0) {
		rtc_stop_wakeup();
		return;
	}
	RTC_WakeUpCmd(DISABLE);
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	RTC_SetWakeUpCounter(seconds - 1);
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line20);
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	RTC_WakeUpCmd(ENABLE);
}

void rtc_stop_wakeup(void) {
	RTC_WakeUpCmd(DISABLE);
	RTC_ITConfig(RTC_IT_WUT, DISABLE);
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line20);
}

void rtc_wakeup_irq(void) {
	if (RTC_GetITStatus(RTC_IT_WUT) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_WUT);
		task_post_pure_msg(MC_CLOCK_RTC_ID, MC_CLOCK_RTC_TICK_1S);
	}
	EXTI_ClearITPendingBit(EXTI_Line20);
}

void rtc_alarm_irq(void) {
	if (RTC_GetITStatus(RTC_IT_ALRA) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		task_post_pure_msg(MC_CLOCK_RTC_ID, MC_CLOCK_RTC_ALARM_FIRED);
	}
	EXTI_ClearITPendingBit(EXTI_Line17);
}

static ErrorStatus rtc_calendar_init(void) {
	RTC_InitTypeDef init;
	init.RTC_HourFormat   = RTC_HourFormat_24;
	init.RTC_AsynchPrediv = 0x7F;
	init.RTC_SynchPrediv  = 0xFF;
	return RTC_Init(&init);
}

static void rtc_seed_default_time(void) {
	RTC_DateTypeDef d0;
	d0.RTC_WeekDay = 4;
	d0.RTC_Date    = 1;
	d0.RTC_Month   = 1;
	d0.RTC_Year    = 26;
	RTC_SetDate(RTC_Format_BIN, &d0);

	RTC_TimeTypeDef t0;
	t0.RTC_H12     = RTC_H12_AM;
	t0.RTC_Hours   = 0;
	t0.RTC_Minutes = 0;
	t0.RTC_Seconds = 0;
	RTC_SetTime(RTC_Format_BIN, &t0);
}

static void rtc_config_exti_and_nvic(void) {
	EXTI_InitTypeDef exti;
	exti.EXTI_Line    = EXTI_Line17 | EXTI_Line20;
	exti.EXTI_Mode    = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannelCmd                = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 2;
	nvic.NVIC_IRQChannelSubPriority        = 0;

	nvic.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_Init(&nvic);

	nvic.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_Init(&nvic);
}
