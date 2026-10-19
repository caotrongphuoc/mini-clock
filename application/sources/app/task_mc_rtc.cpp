#include "task_mc_rtc.h"

/*****************************************************************************/
/* Variable Declaration - MC RTC task */
/*****************************************************************************/
/* A write the RTC did not acknowledge (bus error, RTC backing off) is kept and
 * retried on each 1 Hz tick, so a request is never lost silently. */
#define RTC_WRITE_RETRY_MAX (10)

typedef enum
{
	RTC_ALARM_OP_NONE = 0,
	RTC_ALARM_OP_SET,
	RTC_ALARM_OP_CLEAR,
} mc_clock_rtc_alarm_op_t;

static mc_clock_rtc_set_time_req_t rtc_pending_time;
static uint8_t rtc_pending_time_tries; /* 0 = nothing pending */

static mc_clock_rtc_set_alarm_req_t rtc_pending_alarm;
static uint8_t rtc_pending_alarm_op;
static uint8_t rtc_pending_alarm_tries;

/* A retry happens one tick later, so the requested time moves on by 1 s */
static void mc_clock_rtc_advance_one_second(mc_clock_rtc_set_time_req_t* req)
{
	if (++req->time.sec < 60)
	{
		return;
	}
	req->time.sec = 0;

	if (++req->time.min < 60)
	{
		return;
	}
	req->time.min = 0;

	if (++req->time.hour < 24)
	{
		return;
	}
	req->time.hour = 0;

	req->date.weekday = (req->date.weekday >= RTC_WEEKDAY_SUN) ? RTC_WEEKDAY_MON : req->date.weekday + 1;

	if (++req->date.date <= mc_clock_date_days_in_month(req->date.year, req->date.month))
	{
		return;
	}
	req->date.date = 1;

	if (++req->date.month <= 12)
	{
		return;
	}
	req->date.month = 1;
	req->date.year++;
}

static uint8_t mc_clock_rtc_write_time(const mc_clock_rtc_set_time_req_t* req)
{
	if (req->apply_time && !rtc_set_time(&req->time))
	{
		return 0;
	}

	if (req->apply_date && !rtc_set_date(&req->date))
	{
		return 0;
	}

	return 1;
}

static void mc_clock_rtc_try_time(void)
{
	if (mc_clock_rtc_write_time(&rtc_pending_time))
	{
		rtc_pending_time_tries = 0;
		/* The next alarm depends on the current time */
		task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_REARM);
		return;
	}

	if (++rtc_pending_time_tries > RTC_WRITE_RETRY_MAX)
	{
		APP_DBG("[RTC] set time dropped after %u tries\n", RTC_WRITE_RETRY_MAX);
		rtc_pending_time_tries = 0;
	}
}

static void mc_clock_rtc_try_alarm(void)
{
	uint8_t ok;

	if (rtc_pending_alarm_op == RTC_ALARM_OP_SET)
	{
		ok = rtc_set_alarm_a(&rtc_pending_alarm.time, rtc_pending_alarm.weekday);
	}
	else
	{
		ok = rtc_clear_alarm_a();
	}

	if (ok || ++rtc_pending_alarm_tries > RTC_WRITE_RETRY_MAX)
	{
		if (!ok)
		{
			APP_DBG("[RTC] alarm update dropped after %u tries\n", RTC_WRITE_RETRY_MAX);
		}
		rtc_pending_alarm_op = RTC_ALARM_OP_NONE;
		rtc_pending_alarm_tries = 0;
	}
}

/* The latest alarm request replaces any pending one */
static void mc_clock_rtc_queue_alarm(uint8_t op, const mc_clock_rtc_set_alarm_req_t* req)
{
	rtc_pending_alarm_op = op;
	rtc_pending_alarm_tries = 0;
	if (req)
	{
		rtc_pending_alarm = *req;
	}
	mc_clock_rtc_try_alarm();
}

/*****************************************************************************/
/* Handle - MC RTC task */
/*****************************************************************************/

void task_mc_rtc(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	/* 1 Hz service tick (app_start_timer). The PCF85063A INT pin is not routed
	 * to an EXTI line, so the alarm flag is polled here instead. */
	case MC_CLOCK_RTC_TICK_1S:
	{
		APP_DBG_SIG("MC_CLOCK_RTC_TICK_1S\n");

		if (rtc_pending_time_tries)
		{
			mc_clock_rtc_advance_one_second(&rtc_pending_time);
			mc_clock_rtc_try_time();
		}

		if (rtc_pending_alarm_op != RTC_ALARM_OP_NONE)
		{
			mc_clock_rtc_try_alarm();
		}

		if (rtc_take_alarm_a_flag())
		{
			APP_DBG("[RTC] ALARM_A fired\n");
			task_post_pure_msg(MC_CLOCK_ALARM_ID, MC_CLOCK_ALARM_FIRED);
		}

		/* Background second for objects that must keep running off-screen */
		task_post_pure_msg(MC_CLOCK_TIMER_ID, MC_CLOCK_TIMER_UPDATE);
		task_post_pure_msg(MC_CLOCK_CALENDAR_ID, MC_CLOCK_CALENDAR_UPDATE);
	}
	break;

	case MC_CLOCK_RTC_SET_TIME_REQ:
	{
		APP_DBG_SIG("MC_CLOCK_RTC_SET_TIME_REQ\n");
		if (get_data_len_common_msg(msg) != sizeof(mc_clock_rtc_set_time_req_t))
		{
			break;
		}

		rtc_pending_time = *(mc_clock_rtc_set_time_req_t*)get_data_common_msg(msg);
		rtc_pending_time_tries = 1;
		mc_clock_rtc_try_time();
	}
	break;

	case MC_CLOCK_RTC_SET_ALARM_REQ:
	{
		APP_DBG_SIG("MC_CLOCK_RTC_SET_ALARM_REQ\n");
		if (get_data_len_common_msg(msg) != sizeof(mc_clock_rtc_set_alarm_req_t))
		{
			break;
		}

		mc_clock_rtc_queue_alarm(RTC_ALARM_OP_SET,
		                         (mc_clock_rtc_set_alarm_req_t*)get_data_common_msg(msg));
	}
	break;

	case MC_CLOCK_RTC_CLEAR_ALARM_REQ:
	{
		APP_DBG_SIG("MC_CLOCK_RTC_CLEAR_ALARM_REQ\n");
		mc_clock_rtc_queue_alarm(RTC_ALARM_OP_CLEAR, NULL);
	}
	break;

	default:
		break;
	}
}
