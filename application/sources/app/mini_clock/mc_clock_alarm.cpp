#include "mc_clock_alarm.h"
#include "mc_clock_world.h"
#include "eeprom.h"

#include <string.h>
#include "app_eeprom.h"
#include "mc_clock_ring.h"

#define MC_CLOCK_ALARM_CHECKSUM_SIZE (sizeof(uint32_t) + sizeof(mc_clock_alarm_bank_t))

typedef struct
{
	uint32_t              magic_number;
	mc_clock_alarm_bank_t data;
	uint8_t               check_sum;
} mc_clock_alarm_eeprom_t;

/* Variable Declaration - Clock alarm object */
static mc_clock_alarm_state_t mc_clock_alarm_state = {
    .current_item = 0,
    .scroll_offset = 0,
    .total_alarm = 0,

    .editing = 0,
    .editing_alarm = 0,
    .editing_field = MC_CLOCK_ALARM_EDIT_HOUR,

    .ringing = 0,

    .sound = BUZZER_SOUND_ALARM_CLASSIC,
};

/* Write the alarm bank, skipping the write when EEPROM already holds it */
void mc_clock_alarm_save_to_eeprom(void)
{
	mc_clock_alarm_eeprom_t eeprom_data;
	mc_clock_alarm_eeprom_t stored;

	memset(&eeprom_data, 0, sizeof(eeprom_data));
	eeprom_data.data.total_alarm = mc_clock_alarm_state.total_alarm;
	for (uint8_t i = 0; i < MC_CLOCK_ALARM_MAX; i++)
	{
		eeprom_data.data.alarm[i] = mc_clock_alarm_state.alarm[i];
	}

	mc_clock_eeprom_update_checksum(&eeprom_data.magic_number, &eeprom_data.check_sum, MC_CLOCK_ALARM_CHECKSUM_SIZE);

	if (eeprom_read(EEPROM_ALARM_START_ADDR, (uint8_t*)&stored, sizeof(stored)) == EEPROM_DRIVER_OK &&
	    memcmp(&stored, &eeprom_data, sizeof(stored)) == 0)
	{
		return;
	}

	eeprom_write(EEPROM_ALARM_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data));
}

void mc_clock_alarm_load_from_eeprom(void)
{
	mc_clock_alarm_eeprom_t eeprom_data;

	uint8_t ret = eeprom_read(EEPROM_ALARM_START_ADDR, (uint8_t*)&eeprom_data, sizeof(eeprom_data));

	if (ret == EEPROM_DRIVER_OK &&
	    mc_clock_eeprom_is_valid(&eeprom_data.magic_number, eeprom_data.check_sum, MC_CLOCK_ALARM_CHECKSUM_SIZE))
	{
		mc_clock_alarm_state.total_alarm = eeprom_data.data.total_alarm;
		if (mc_clock_alarm_state.total_alarm > MC_CLOCK_ALARM_MAX)
		{
			mc_clock_alarm_state.total_alarm = 0;
		}
		for (uint8_t i = 0; i < MC_CLOCK_ALARM_MAX; i++)
		{
			mc_clock_alarm_state.alarm[i] = eeprom_data.data.alarm[i];
		}
	}

	mc_clock_alarm_apply_rtc();
}

void mc_clock_alarm_set_sound(uint8_t sound)
{
    mc_clock_alarm_state.sound = (buzzer_sound_t)sound;
}

uint8_t mc_clock_alarm_get_sound(void)
{
    return (uint8_t)mc_clock_alarm_state.sound;
}
/* Public API - Clock alarm object */
void mc_clock_alarm_get_state(mc_clock_alarm_state_t* state)
{
	*state = mc_clock_alarm_state;
}

void mc_clock_alarm_apply_rtc(void)
{
	rtc_time_t now;
	uint16_t now_min;
	uint16_t best_delta = 24 * 60;
	uint16_t best_a_min_rtc = 0;
	uint8_t found = 0;

	rtc_get_time(&now);
	now_min = now.hour * 60 + now.min;

	int16_t offset_minutes = mc_clock_world_clock_get_selected_offset_minutes();

	for (uint8_t i = 0; i < mc_clock_alarm_state.total_alarm; i++)
	{
		if (!mc_clock_alarm_state.alarm[i].enabled)
		{
			continue;
		}

		// Convert local country alarm time to base RTC timezone time
		int32_t a_min_rtc = (int32_t)mc_clock_alarm_state.alarm[i].hour * 60 + mc_clock_alarm_state.alarm[i].minute - offset_minutes;
		a_min_rtc = ((a_min_rtc % 1440) + 1440) % 1440;

		uint16_t delta = (a_min_rtc > now_min) ? (a_min_rtc - now_min) : (24 * 60 - (now_min - a_min_rtc));

		if (!found || delta < best_delta)
		{
			found = 1;
			best_delta = delta;
			best_a_min_rtc = (uint16_t)a_min_rtc;
		}
	}

	if (!found)
	{
		task_post_pure_msg(MC_CLOCK_RTC_ID, MC_CLOCK_RTC_CLEAR_ALARM_REQ);
		return;
	}

	mc_clock_rtc_set_alarm_req_t req;
	req.time.hour = (uint8_t)(best_a_min_rtc / 60);
	req.time.min = (uint8_t)(best_a_min_rtc % 60);
	req.time.sec = 0;
	req.weekday = RTC_ALARM_ANY_WEEKDAY;

	task_post_common_msg(MC_CLOCK_RTC_ID,
	                     MC_CLOCK_RTC_SET_ALARM_REQ,
	                     (uint8_t*)&req,
	                     sizeof(req));
}



/* Copy of the alarm being edited, restored when the edit is abandoned */
static mc_clock_alarm_item_t mc_clock_alarm_edit_backup;
static uint8_t mc_clock_alarm_editing_new;

/* The alarm list changed: re-arm the RTC and persist the bank */
static void mc_clock_alarm_commit(void)
{
	mc_clock_alarm_apply_rtc();
	mc_clock_alarm_save_to_eeprom();
}

static void mc_clock_alarm_begin_edit(uint8_t index, uint8_t is_new)
{
	mc_clock_alarm_state.editing = 1;
	mc_clock_alarm_state.editing_alarm = index;
	mc_clock_alarm_state.editing_field = MC_CLOCK_ALARM_EDIT_HOUR;
	mc_clock_alarm_edit_backup = mc_clock_alarm_state.alarm[index];
	mc_clock_alarm_editing_new = is_new;
}

static void mc_clock_alarm_scroll_to_current(void)
{
	if (mc_clock_alarm_state.current_item >= mc_clock_alarm_state.total_alarm)
	{
		return;
	}

	if (mc_clock_alarm_state.current_item < mc_clock_alarm_state.scroll_offset)
	{
		mc_clock_alarm_state.scroll_offset = mc_clock_alarm_state.current_item;
	}
	else if (mc_clock_alarm_state.current_item >= mc_clock_alarm_state.scroll_offset + 3)
	{
		mc_clock_alarm_state.scroll_offset = mc_clock_alarm_state.current_item - 2;
	}
}

static void mc_clock_alarm_delete_current()
{
	uint8_t index = mc_clock_alarm_state.editing_alarm;

	if (index >= mc_clock_alarm_state.total_alarm)
	{
		return;
	}

	// Move all alarms after it one position up
	for (uint8_t i = index; i + 1 < mc_clock_alarm_state.total_alarm; i++)
	{
		mc_clock_alarm_state.alarm[i] =
		    mc_clock_alarm_state.alarm[i + 1];
	}

	// Remove last item
	mc_clock_alarm_state.total_alarm--;

	// Fix selected item
	if (mc_clock_alarm_state.current_item >=
	    mc_clock_alarm_state.total_alarm)
	{
		if (mc_clock_alarm_state.total_alarm > 0)
		{
			mc_clock_alarm_state.current_item =
			    mc_clock_alarm_state.total_alarm - 1;
		}
		else
		{
			mc_clock_alarm_state.current_item = 0;
		}
	}

	mc_clock_alarm_state.editing = 0;
	mc_clock_alarm_state.editing_field = MC_CLOCK_ALARM_EDIT_HOUR;

	mc_clock_alarm_commit();
}

static void mc_clock_alarm_stop_ringing(void)
{
	mc_clock_alarm_state.ringing = 0;
	BUZZER_Disable();
	/* Re-arm the RTC for the next enabled alarm */
	mc_clock_alarm_apply_rtc();
}

void mc_clock_alarm_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case MC_CLOCK_ALARM_SETUP:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_SETUP\n");

		/* The screen was left mid-edit: discard the unsaved change */
		if (mc_clock_alarm_state.editing &&
		    mc_clock_alarm_state.editing_alarm < mc_clock_alarm_state.total_alarm)
		{
			if (mc_clock_alarm_editing_new)
			{
				mc_clock_alarm_state.total_alarm--;
			}
			else
			{
				mc_clock_alarm_state.alarm[mc_clock_alarm_state.editing_alarm] = mc_clock_alarm_edit_backup;
			}
		}
		mc_clock_alarm_editing_new = 0;

		mc_clock_alarm_state.current_item = 0;
		mc_clock_alarm_state.scroll_offset = 0;
		mc_clock_alarm_state.editing = 0;
		mc_clock_alarm_state.editing_field = MC_CLOCK_ALARM_EDIT_HOUR;
	}
	break;

	case MC_CLOCK_ALARM_SELECT:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_SELECT\n");

		if (mc_clock_alarm_state.editing)
		{
			switch (mc_clock_alarm_state.editing_field)
			{
			case MC_CLOCK_ALARM_EDIT_DELETE:
			{
				mc_clock_alarm_delete_current();
			}
			break;

			case MC_CLOCK_ALARM_EDIT_SAVE:
			{
				mc_clock_alarm_state.editing = 0;
				mc_clock_alarm_commit();
			}
			break;

			default:
				mc_clock_alarm_state.editing_field =
				    (mc_clock_alarm_edit_field_t)(mc_clock_alarm_state.editing_field + 1);
				break;
			}

			break;
		}

		// Add New button
		if (mc_clock_alarm_state.current_item == mc_clock_alarm_state.total_alarm)
		{
			if (mc_clock_alarm_state.total_alarm < MC_CLOCK_ALARM_MAX)
			{
				uint8_t new_alarm = mc_clock_alarm_state.total_alarm;

				mc_clock_alarm_state.alarm[new_alarm].hour = 8;
				mc_clock_alarm_state.alarm[new_alarm].minute = 0;
				mc_clock_alarm_state.alarm[new_alarm].enabled = 1;

				mc_clock_alarm_state.total_alarm++;

				mc_clock_alarm_state.current_item = new_alarm;
				mc_clock_alarm_scroll_to_current();

				mc_clock_alarm_commit();
			}

			break;
		}

		// Back button
		if (mc_clock_alarm_state.current_item == mc_clock_alarm_state.total_alarm + 1)
		{
			break;
		}

		// Existing alarm selected
		if (mc_clock_alarm_state.current_item < mc_clock_alarm_state.total_alarm)
		{
			mc_clock_alarm_begin_edit(mc_clock_alarm_state.current_item, 0);
		}
	}
	break;

	case MC_CLOCK_ALARM_UP:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_UP\n");
		if (mc_clock_alarm_state.editing)
		{
			mc_clock_alarm_item_t* alarm =
			    &mc_clock_alarm_state.alarm[mc_clock_alarm_state.editing_alarm];

			switch (mc_clock_alarm_state.editing_field)
			{
			case MC_CLOCK_ALARM_EDIT_HOUR:
				alarm->hour = (alarm->hour + 1) % 24;
				break;

			case MC_CLOCK_ALARM_EDIT_MINUTE:
				alarm->minute = (alarm->minute + 1) % 60;
				break;

			case MC_CLOCK_ALARM_EDIT_ENABLE:
				alarm->enabled = 1;
				break;

			default:
				break;
			}

			break;
		}

		if (mc_clock_alarm_state.current_item == 0)
		{
			mc_clock_alarm_state.current_item = mc_clock_alarm_state.total_alarm + 1;
		}
		else
		{
			mc_clock_alarm_state.current_item--;
		}

		mc_clock_alarm_scroll_to_current();
	}
	break;

	case MC_CLOCK_ALARM_DOWN:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_DOWN\n");
		if (mc_clock_alarm_state.editing)
		{
			mc_clock_alarm_item_t* alarm =
			    &mc_clock_alarm_state.alarm[mc_clock_alarm_state.editing_alarm];

			switch (mc_clock_alarm_state.editing_field)
			{
			case MC_CLOCK_ALARM_EDIT_HOUR:
				alarm->hour =
				    (alarm->hour == 0) ? 23 : alarm->hour - 1;
				break;

			case MC_CLOCK_ALARM_EDIT_MINUTE:
				alarm->minute =
				    (alarm->minute == 0) ? 59 : alarm->minute - 1;
				break;

			case MC_CLOCK_ALARM_EDIT_ENABLE:
				alarm->enabled = 0;
				break;

			default:
				break;
			}

			break;
		}

		mc_clock_alarm_state.current_item++;
		if (mc_clock_alarm_state.current_item > mc_clock_alarm_state.total_alarm + 1)
		{
			mc_clock_alarm_state.current_item = 0;
		}

		mc_clock_alarm_scroll_to_current();
	}
	break;

	case MC_CLOCK_ALARM_NEXT_FIELD:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_NEXT_FIELD\n");

		if (mc_clock_alarm_state.editing)
		{
			if (mc_clock_alarm_state.editing_field == MC_CLOCK_ALARM_EDIT_SAVE)
			{
				mc_clock_alarm_state.editing = 0;
				mc_clock_alarm_state.editing_field = MC_CLOCK_ALARM_EDIT_HOUR;

				mc_clock_alarm_commit();
			}
			else
			{
				mc_clock_alarm_state.editing_field =
				    (mc_clock_alarm_edit_field_t)(mc_clock_alarm_state.editing_field + 1);
			}
		}
	}
	break;

	case MC_CLOCK_ALARM_ADD_NEW:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_ADD_NEW\n");

		if (mc_clock_alarm_state.total_alarm < MC_CLOCK_ALARM_MAX)
		{
			uint8_t new_alarm = mc_clock_alarm_state.total_alarm;

			mc_clock_alarm_state.alarm[new_alarm].hour = 8;
			mc_clock_alarm_state.alarm[new_alarm].minute = 0;
			mc_clock_alarm_state.alarm[new_alarm].enabled = 1;

			mc_clock_alarm_state.total_alarm++;

			mc_clock_alarm_state.current_item = new_alarm;
			mc_clock_alarm_scroll_to_current();

			// enter editing immediately; saved only when the edit is confirmed
			mc_clock_alarm_begin_edit(new_alarm, 1);
		}
	}
	break;

	case MC_CLOCK_ALARM_TOGGLE:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_TOGGLE\n");
		if (mc_clock_alarm_state.editing == 0 &&
		    mc_clock_alarm_state.current_item < mc_clock_alarm_state.total_alarm)
		{
			mc_clock_alarm_item_t* alarm = &mc_clock_alarm_state.alarm[mc_clock_alarm_state.current_item];
			alarm->enabled = !alarm->enabled;
			mc_clock_alarm_commit();
		}
	}
	break;

	case MC_CLOCK_ALARM_DELETE:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_DELETE\n");
		if (mc_clock_alarm_state.editing == 0 &&
		    mc_clock_alarm_state.current_item < mc_clock_alarm_state.total_alarm)
		{
			for (uint8_t i = mc_clock_alarm_state.current_item; i + 1 < mc_clock_alarm_state.total_alarm; i++)
			{
				mc_clock_alarm_state.alarm[i] = mc_clock_alarm_state.alarm[i + 1];
			}

			mc_clock_alarm_state.total_alarm--;

			/* current_item now points at the next alarm (or "Add New");
			 * keep the 3-line window inside the shorter list. */
			uint8_t max_offset = (mc_clock_alarm_state.total_alarm > 3) ? (mc_clock_alarm_state.total_alarm - 3) : 0;
			if (mc_clock_alarm_state.scroll_offset > max_offset)
			{
				mc_clock_alarm_state.scroll_offset = max_offset;
			}
			mc_clock_alarm_scroll_to_current();

			mc_clock_alarm_commit();
		}
	}
	break;

	case MC_CLOCK_ALARM_FIRED:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_FIRED\n");
		if (mc_clock_alarm_state.ringing)
		{
			break;
		}

		mc_clock_alarm_state.ringing = 1;
		BUZZER_PlaySound(mc_clock_alarm_state.sound);
		mc_clock_ring_request(MC_CLOCK_RING_SOURCE_ALARM, mc_clock_alarm_state.sound);
	}
	break;

	case MC_CLOCK_ALARM_DISMISS:
	{
		APP_DBG_SIG("MC_CLOCK_ALARM_DISMISS\n");
		mc_clock_alarm_stop_ringing();
	}
	break;

	default:
		break;
	}
}
