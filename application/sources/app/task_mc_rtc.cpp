#include "ak.h"
#include "message.h"

#include "app.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_mc_rtc.h"

#include "rtc.h"

/*****************************************************************************/
/* Handle - MC RTC task */
/*****************************************************************************/

void task_mc_rtc(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_RTC_TICK_1S:
	{
		rtc_time_t time;

		rtc_get_time(&time);
		APP_DBG("[RTC] %02u:%02u:%02u\n", time.hour, time.min, time.sec);
		task_post_pure_msg(AC_TASK_DISPLAY_ID, MC_CLOCK_TIME_TICK);
	}
	break;

	case MC_CLOCK_RTC_ALARM_FIRED:
	{
		APP_DBG("[RTC] ALARM_A fired\n");
	}
	break;

	case MC_CLOCK_RTC_SET_TIME_REQ:
	{
		mc_clock_rtc_set_time_req_t* req =
		    (mc_clock_rtc_set_time_req_t*)get_data_common_msg(msg);

		if (req->apply_time)
		{
			rtc_set_time(&req->time);
		}

		if (req->apply_date)
		{
			rtc_set_date(&req->date);
		}
	}
	break;

	case MC_CLOCK_RTC_SET_ALARM_REQ:
	{
		mc_clock_rtc_set_alarm_req_t* req =
		    (mc_clock_rtc_set_alarm_req_t*)get_data_common_msg(msg);

		rtc_set_alarm_a(&req->time, req->weekday);
	}
	break;

	case MC_CLOCK_RTC_CLEAR_ALARM_REQ:
	{
		rtc_clear_alarm_a();
	}
	break;

	case MC_CLOCK_RTC_SET_WAKEUP_REQ:
	{
		mc_clock_rtc_set_wakeup_req_t* req =
		    (mc_clock_rtc_set_wakeup_req_t*)get_data_common_msg(msg);

		rtc_set_wakeup(req->seconds);
	}
	break;

	default:
		break;
	}
}
