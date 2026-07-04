#ifndef __MC_CLOCK_CLOCK_H__
#define __MC_CLOCK_CLOCK_H__

#include "ak.h"
#include "rtc.h"

void mc_clock_clock_handle(ak_msg_t* msg);

void mc_clock_clock_get_time(rtc_time_t* time);
void mc_clock_clock_get_date(rtc_date_t* date);
uint8_t mc_clock_clock_is_24h_format();

#endif /* __MC_CLOCK_CLOCK_H__ */
