# AK firmware — agent instructions

This project is firmware for the **AK (Active Kernel)** event-driven MCU framework.

You have access to the **`ak-docs` MCP server**. Use it as the source of truth — do
not guess AK APIs from memory. (Codex: `~/.codex/config.toml`. Claude Code: `.mcp.json`
in this repo; check with `/mcp`.)

Before writing or changing any firmware:

1. Call **`get_ak_guardrails`** first. Never modify `application/sources/ak/` (kernel),
   `boot/`, `application/sources/networks/`, or `application/sources/common/`.
   Build features in `application/sources/app/` and `application/sources/driver/` only.
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
  `mc_clock_alarm_apply_rtc()`, `mc_clock_alarm_get_sound()`, `mc_clock_time_adjust_timezone()`,
  setting apply/reset helpers) are legacy. Do not add more; when touching that flow, prefer
  moving it to messages and say so in the change summary.
- The active screen owns its periodic tick on `AC_TASK_DISPLAY_ID`, sets it on
  `SCREEN_ENTRY`, and must `timer_remove_attr()` it before every `SCREEN_TRAN`/`SCREEN_BACK`.
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

## Known incomplete items (verify before relying on them)

- RTC alarm never fires: `rtc_alarm_irq()` is empty and nothing polls the PCF85063A AF flag,
  so `MC_CLOCK_RTC_ALARM_FIRED` is never posted. No UI posts `MC_CLOCK_ALARM_DISMISS`.
- `app_task_init()` calls `mc_clock_alarm_apply_rtc()` before EEPROM is loaded, which saves an
  empty alarm bank over the stored one on every boot.
- Timer, stopwatch, and calendar event alarms only advance while their own screen is open.
- Calendar event alarm ignores `alarm_enabled`; `repeat` and `alarm_sound` fields are unused.
- `MC_CLOCK_RTC_TICK_1S`, `MC_CLOCK_RTC_WAKEUP`, `MC_CLOCK_RTC_SET_DATE_REQ` are never posted.
- RAM is ~87% used (~2 KB stack). Avoid large stack copies (the world-clock snapshot is ~1 KB).

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
  echoes commands but `ver` / `fatal l` print nothing. A diagnostic build that enables them is
  temporary and must be restored. Silence is not proof of a healthy board.
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
