# AK firmware — agent instructions

This project is firmware for the **AK (Active Kernel)** event-driven MCU framework.

You have access to the **`ak-docs` MCP server**. Use it as the source of truth — do
not guess AK APIs from memory. (Codex: `~/.codex/config.toml`. Claude Code: `.mcp.json`
in this repo; check with `/mcp`.)

Before writing or changing any firmware:

1. Call **`get_ak_guardrails`** first. Never modify `application/sources/ak/` (kernel;
   `ak.cfg.mk` pool sizing is the exception), `boot/`, `application/sources/sys/`,
   `application/sources/networks/`, `application/sources/common/`,
   `application/sources/libraries/`, or `application/sources/platform/` (except
   `platform/stm32l/io_cfg.c/.h`). Build features in `application/sources/app/` and
   `application/sources/driver/`; feature flags go in `application/Makefile`.
   All GPIO/pin/register code lives only in `io_cfg.c/.h` and is injected into drivers as
   function pointers. ISRs only post messages (see `get_ak_guide("isr-bridge")`).
   The `task_*` prefix is reserved for handlers registered in `task_list.h`.
2. For a new task/driver/screen, call **`get_ak_guide`** (`create-task`,
   `create-driver`, `create-screen`, `use-timer`, `isr-bridge`, `tune-pools`) and follow
   the steps and skeleton exactly.
3. For any kernel function/macro, call **`get_ak_api`** to get the exact signature and
   arguments. Use **`search_ak_docs`** when you don't know the symbol name.

**Debugging a running board?** All debugging is over the UART console (115200 8N1):
call **`get_ak_guide("debug-uart-shell")`** first. Capture output non-interactively with
`python ak-console.py --port <P> --cmd "ver" --cmd "fatal l"` (or `--watch 15` for live
logs), then paste the raw text into **`analyze_ak_log`** and follow its Next steps.
For display issues, capture `--cmd "lcd d"` and paste the dump into **`decode_ak_lcd`**.
Run only read-only shell commands on your own; destructive ones (`reboot`, `fatal t/!/@/r`,
`ram r`, `eps r`, `flash i`, `boot r/t`, `fwu`, `dbg s`) need the engineer's explicit OK.

Hard rules (also returned by `get_ak_guardrails`):

- Handlers must be **non-blocking** — no `delay()`, no busy-wait. Use a timer that posts a
  signal instead.
- Tasks communicate **only via messages** (`task_post_*`), never direct cross-task calls
  or foreign-state access.
- User signals start at `AK_USER_DEFINE_SIG` (10); task priorities are `LEVEL_1..7`
  (0 is reserved).
- Common message payload ≤ 64 bytes; max 7 references per message; pools are fixed size.

# Mini Clock project instructions

## Project context

Digital desk clock on the **AK Embedded Base Kit** (STM32L151CBT6, 16 KB RAM, 128x64
OLED, 3 buttons Mode/Up/Down, buzzer, external **PCF85063A RTC over I2C**, EEPROM).
Prefix: `mc_clock_*` (Mini Clock).

## Layout and naming

- Clock objects: `application/sources/app/mini_clock/mc_clock_<object>.h/.cpp`
  (time, stopwatch, timer, alarm, world, calendar). Build list: `mini_clock/Makefile.mk`.
- RTC bridge task: `application/sources/app/task_mc_rtc.h/.cpp` (owns all RTC writes).
- Screens: `application/sources/app/screens/scr_clock_<name>.h/.cpp`, declared in `screens.h`.
- RTC driver: `application/sources/driver/rtc/rtc.cpp` (PCF85063A, not the STM32 internal RTC).
- Persistence: `application/sources/app/app_eeprom.h/.cpp`.
- Signals: `MC_CLOCK_<OBJECT>_<ACTION>`, each object's enum anchored to
  `MC_CLOCK_DEFINE_SIG` (100, defined in `ak.h`). Timer intervals `MC_CLOCK_<OBJECT>_*_INTERVAL`
  live in `app.h`.
- Task IDs: `MC_CLOCK_<NAME>_ID` with handler `mc_clock_<name>_handle` (RTC task:
  `task_mc_rtc`). Keep `task_list.h` enum order identical to `app_task_table[]`.
- Typedefs: `mc_clock_<object>_state_t` / `mc_<object>_*_t`. Header guards `__MC_CLOCK_<OBJECT>_H__`,
  `__SCR_CLOCK_<NAME>_H__`.

## Ownership and runtime model (current code)

- Each object keeps its state `static` in its `.cpp` and exposes `mc_clock_<object>_get_state()`
  (copy-out snapshot). Screens read snapshots for rendering and post `MC_CLOCK_*` signals
  for every state change. Do not add new writes into another object's state.
- Existing direct calls across objects (`mc_clock_world_clock_get_selected_offset_minutes()`,
  `mc_clock_world_clock_get_country()`, `mc_clock_alarm_get_sound()`,
  `mc_clock_time_adjust_timezone()`, setting apply/reset helpers, shell debug commands) are
  legacy. Do not add more; when touching that flow, prefer moving it to messages and say so in
  the change summary. Alarm re-arming is message-only: post `MC_CLOCK_ALARM_REARM` after the
  time or time zone changes (`mc_clock_alarm_apply_rtc()` is private).
- The active screen owns its periodic tick on `AC_TASK_DISPLAY_ID`, sets it on
  `SCREEN_ENTRY`, and must `timer_remove_attr()` it before every `SCREEN_TRAN`/`SCREEN_BACK`.
  Screen ticks only refresh the view; animation ticks (menu, world clock) run only while sliding.
  Many display signals share the value 100 (`MC_CLOCK_TIME_TICK`, `MC_CLOCK_TIMER_TICK`,
  `MC_CLOCK_STOPWATCH_TICK`), so never post one of them to the display from outside a screen.
- **1 Hz service tick:** `task_mc_rtc` receives `MC_CLOCK_RTC_TICK_1S` (started in
  `app_start_timer`). It polls the PCF85063A alarm flag (the INT pin is not used), retries RTC
  writes the chip did not acknowledge (max 10), and posts `MC_CLOCK_TIMER_UPDATE` and
  `MC_CLOCK_CALENDAR_UPDATE` so the timer and reminders run on every screen. The stopwatch owns
  its own 100 ms timer while running.
- **Ringing:** a source (alarm, timer, calendar) plays the first sound and calls
  `mc_clock_ring_request()` (`mini_clock/mc_clock_ring.h`), which posts `AC_DISPLAY_SHOW_RINGING`.
  `task_display` switches to `scr_clock_ringing` from any screen; that screen repeats the sound,
  times out after 60 s and posts the source's dismiss signal.
- After an object updates the time it shows (time, world, timer), it posts
  `AC_DISPLAY_RENDER_SCREEN` so the screen redraws with the new value instead of the old one.
- RAM is tight (13.3 of 16 KB, ~3 KB stack, 2 KB heap). Keep snapshots small; tables that do
  not change belong in flash (`static const`).
- RTC holds **Vietnam local time (UTC+7)**. World-clock offsets are minutes relative to UTC+7.
  The selected country is the device's display timezone; alarms and calendar events are stored
  in that local time and converted to raw RTC time when programming the RTC.
- Validate common-message payload length before casting `get_data_common_msg()`.

## Persistence (EEPROM)

| Record | Address | Content |
|---|---|---|
| Settings | `0x0010` | `mc_clock_setting_t` (12h, mute, chime, alarm sound, country, brightness, invert) |
| Alarm bank | `0x0100` | `mc_clock_alarm_bank_t` (total + 10 alarms) |
| Calendar bank | `0x0200` | `mc_calendar_bank_t` (total + 10 events) |

- Every record is `magic (0x6D63636B "mcck") + data + 8-bit sum checksum`. Invalid → defaults.
- Changing a persisted struct changes its size/layout: keep the record inside its address
  window, and expect old data to be rejected (defaults) after flashing.
- Never erase or reset EEPROM/RTC on the board without the engineer's OK.

## Controls (current behaviour — ask before changing)

- Main/Analog/World: Mode → menu. Menu: Up/Down cycle, Mode enter.
- Alarm: Up/Down move or edit, Mode select/next, long Mode next field, long Up toggle,
  long Down delete, Mode+Up add, Mode+Down back.
- Stopwatch: Mode start/pause, long Mode reset, Up lap, Down scroll laps, long Up/Down back.
- Timer: Mode start/pause/dismiss, Up/Down edit field, Mode+Up/Down change field,
  long Up reset, long Down back.
- Calendar: Up/Down move day/field, long Up/Down change month or back, Mode open/next,
  long Mode add/delete/finish, Mode+Up back to menu.
- Ringing screen: any key dismisses. Full per-screen table: README "III. Controls".

## Known limitations (verify before relying on them)

- The PCF85063A INT pin is not used; the alarm flag is polled once per second.
- Calendar event fields `repeat` and `alarm_sound` are stored but unused (reminders use the
  alarm melody). Two events in the same minute ring one after the other.
- Not exercised on hardware yet: the RTC write retry path (needs an I2C fault), an empty
  calendar on a blank EEPROM, and re-arming from World Clock / Setting > Reset (same
  `MC_CLOCK_ALARM_REARM` path as the tested time change).
- `twi.cpp` (platform, read-only) leaves I2C1 BUSY after a NACK/timeout; `driver/rtc` recovers
  the bus through `io_i2c1_bus_busy()` / `io_i2c1_bus_recover()` in `io_cfg.c`.

## Build and validation

- Build: `make -C application`. The toolchain defaults to
  `$(WORKSPACE_TOOLS_PATH)/gcc-arm-none-eabi-10.3-2021.10-...` with
  `WORKSPACE_TOOLS_PATH ?= ../../../tools` (i.e. `~/workspace/tools`); override `GCC_PATH`
  or `WORKSPACE_TOOLS_PATH` if needed.
- Before handing off firmware changes: `make -C application clean`, `make -C application`,
  `git diff --check`. Report Flash and RAM usage from the successful build.
- Flash only with the engineer's approval: `make -C application flash dev=/dev/ttyUSB0`
  (AK bootloader via `ak-flash`, app at `0x08003000`).
- All console print/debug gates (`LOGIN_PRINT`, `SYS_PRINT`, `APP_PRINT`, `SYS_DBG`, `APP_DBG`,
  `APP_DBG_SIG`) are disabled in `application/Makefile` for the play build, so the shell
  echoes commands but `ver` / `fatal l` print nothing. For on-device checks set
  `-DLOGIN_PRINT_EN -DSYS_PRINT_EN` temporarily and restore `-U` before handing off. Keep the
  kernel task log (`LOG_AK_KERNEL_ENABLE` in `ak.cfg.mk`) off unless profiling: it prints every
  message and starves the shell. Silence is not proof of a healthy board.
- Console helper: `python3 tools/ak-console.py --port /dev/ttyUSB0 --cmd "ver"` (`--watch`,
  `--key`, `--list`). App shell commands: `stt` (alarm/calendar state), `rtc g` /
  `rtc s YYYYMMDD hhmmss`, `mute 1|0`, `btn m|u|d|M|U|D|a|b` (simulated presses). Verify
  navigation with `stt` rather than guessing from `lcd d`; wait ~1 s between menu presses
  (presses during the slide animation are ignored).
- Restore what a test changes on the board (alarms, events, RTC time, selected city, settings).
- Never claim on-device validation when only compilation was performed.

## Coding style

Use the root `.clang-format`: Allman braces, tab indent 4, `int* p`, `if (x)`, no auto
line-wrap, no include sorting. Run `clang-format -i` on changed C/C++ files only.

## Commit message workflow

After completing any logical unit of work, provide a ready-to-copy commit command:

    git commit -m "[ACTION] short description"

- ACTION in UPPERCASE: ADD / UPDATE / FIX / REMOVE / DOC / MERGE
- Description lowercase, imperative, no trailing period, under ~80 characters
- One topic per commit; name the module/file/signal when relevant
- When following an agreed TODO list: finish one TODO, give its commit command, and wait
  for the engineer to confirm before continuing. Do not stage or commit automatically.
