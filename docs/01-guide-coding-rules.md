<h1 align="center">Coding rules and development guide</h1>

This guide defines Mini Clock's naming, source layout, task ownership, formatting, commit messages
and documentation conventions, and how to build, flash and debug the firmware. Examples use the
existing `mc_clock_*` objects and `scr_clock_*` screens. Apply these rules when extending the clock;
do not reorganize working modules or change firmware behaviour as part of a formatting update.

## Table of contents

- [I. Naming conventions](#i-naming-conventions)
- [II. Ownership and message contracts](#ii-ownership-and-message-contracts)
- [III. Code style (clang-format)](#iii-code-style-clang-format)
- [IV. Build and flash](#iv-build-and-flash)
- [V. UART debug shell](#v-uart-debug-shell)
- [VI. Commit message convention](#vi-commit-message-convention)
- [VII. Document file naming convention](#vii-document-file-naming-convention)
- [VIII. References](#viii-references)

## I. Naming conventions

Use names that identify the owning module and can be searched without guessing abbreviations.

### 1. Folders

| Folder | Content |
|---|---|
| `application/sources/app/mini_clock/` | Clock objects: `mc_clock_<object>.h/.cpp` |
| `application/sources/app/screens/` | Screens: `scr_clock_<name>.h/.cpp`, declared in `screens.h` |
| `application/sources/app/` | Tasks registered in `task_list.h` (`task_mc_rtc.cpp`), signals (`app.h`), shell, EEPROM records |
| `application/sources/driver/` | Hardware-agnostic drivers (`rtc/`, `buzzer/`, ...) |
| `application/sources/platform/stm32l/io_cfg.c/.h` | The only place for GPIO, pin and register code |

Do not modify `ak/` (except `ak.cfg.mk`), `boot/`, `sys/`, `common/`, `networks/`, `libraries/` or
the rest of `platform/`.

### 2. Files and header guards

| Kind | File | Header guard |
|---|---|---|
| Clock object | `mc_clock_alarm.h/.cpp` | `__MC_CLOCK_ALARM_H__` |
| Screen | `scr_clock_ringing.h/.cpp` | `__SCR_CLOCK_RINGING_H__` |
| AK task | `task_mc_rtc.h/.cpp` | `__TASK_MC_RTC_H__` |

The `task_` prefix is reserved for handlers registered in `task_list.h`.

### 3. Signals

Each object's signals are an enum in `app.h` anchored to `MC_CLOCK_DEFINE_SIG` (100):
`MC_CLOCK_<OBJECT>_<ACTION>`, for example `MC_CLOCK_ALARM_SELECT`, `MC_CLOCK_RTC_SET_TIME_REQ`.
Display signals are `AC_DISPLAY_<ACTION>` anchored to `AK_USER_DEFINE_SIG`. Timer intervals are
`#define <SIGNAL>_INTERVAL` next to the enum, in milliseconds.

### 4. Task IDs

`MC_CLOCK_<NAME>_ID` with handler `mc_clock_<name>_handle()`; the RTC task is `MC_CLOCK_RTC_ID`
with `task_mc_rtc()`. Keep the `task_list.h` enum order identical to `app_task_table[]`.

### 5. Types, functions and variables

- Types: `mc_clock_<object>_state_t`, `mc_<object>_<thing>_t`, payloads `mc_clock_<task>_<request>_req_t`.
- Public functions: `mc_clock_<object>_<verb>()`; file-local helpers are `static`.
- Object state is one `static` variable in the object's `.cpp`; constant tables are `static const`
  so they stay in flash.
- Macros: `MC_CLOCK_<OBJECT>_<NAME>` (for example `MC_CLOCK_ALARM_MAX`).

## II. Ownership and message contracts

- An object's state is changed only by its own handler. Other tasks post a signal.
- Screens read `mc_clock_<object>_get_state()` snapshots to render and post signals for every
  change. Keep snapshots small: about 3 KB of stack is left.
- After a handler changes something a screen shows, post `AC_DISPLAY_RENDER_SCREEN`.
- A common-message payload is at most 64 bytes; check `get_data_len_common_msg()` before casting
  `get_data_common_msg()`.
- Handlers never block. Delays are timers; a periodic timer is removed when it is no longer needed.
- RTC writes go through `task_mc_rtc` (`MC_CLOCK_RTC_SET_TIME_REQ`, `SET_ALARM_REQ`,
  `CLEAR_ALARM_REQ`). After the time or the display city changes, post `MC_CLOCK_ALARM_REARM`.
- Something that rings calls `mc_clock_ring_request(source, sound)` and handles its `*_DISMISS`.

## III. Code style (clang-format)

The root [`.clang-format`](../.clang-format) is the source of truth:

| Rule | Setting |
|---|---|
| Base | LLVM |
| Indentation | Tabs, width 4 |
| Braces | Allman (`BreakBeforeBraces: Allman`) |
| Line length | No automatic wrapping (`ColumnLimit: 0`) |
| Pointers | `int* p` (`PointerAlignment: Left`) |
| Short blocks | Never on one line (`if`, functions, `case`) |
| Includes | Not sorted (`SortIncludes: false`) |

```bash
clang-format -i application/sources/app/mini_clock/mc_clock_alarm.cpp
```

Format only the files you change. Several older files do not follow the style yet; reformatting
them in a feature commit hides the real change.

## IV. Build and flash

The application builds with the GNU Arm toolchain 10.3-2021.10. By default the Makefile looks for
it in `~/workspace/tools` (`WORKSPACE_TOOLS_PATH ?= ../../../tools`); override `GCC_PATH` or
`WORKSPACE_TOOLS_PATH` for another location.

```bash
make -C application clean
make -C application                              # build, prints Flash and RAM usage
make -C application flash dev=/dev/ttyUSB0       # flash through the AK bootloader (ak-flash)
```

The application starts at `0x08003000`, after the AK bootloader and the shared boot partition.
Before handing off a change, run a clean build, check `git diff --check`, and note the Flash and RAM
usage (release build: Flash 74,768 / 118,784 bytes, RAM 13,324 / 16,384 bytes).

## V. UART debug shell

The console runs at **115200 8N1**. `tools/ak-console.py` sends commands and prints the replies
without an interactive terminal:

```bash
python3 tools/ak-console.py --list
python3 tools/ak-console.py --port /dev/ttyUSB0 --cmd "ver" --cmd "fatal l"
python3 tools/ak-console.py --port /dev/ttyUSB0 --watch 10
```

| Command | Purpose |
| --- | --- |
| `ver` | Kernel and app version, firmware checksum, stack and heap available. |
| `fatal l` | Last FATAL record and restart counters. |
| `ram s` | Stack size and peak usage. |
| `lcd d` | OLED framebuffer dump (128×64, page-major). |
| `stt` | Alarm and calendar state (list position, edit mode, ringing). |
| `rtc g` / `rtc s YYYYMMDD hhmmss` | Read or set the RTC (Vietnam time). |
| `mute 1` / `mute 0` | Saved mute setting, without the click sounds of the menu. |
| `btn m\|u\|d` | Simulate a press of Mode, Up, or Down. |
| `btn M\|U\|D` | Simulate a long press. |
| `btn a\|b` | Simulate Mode+Up or Mode+Down. |

Wait about 1.5 s between `btn` presses in the menu: a press during the slide animation is ignored.
Check the result with `stt` instead of guessing from the screen.

### Console output gates

The release build compiles the console output out in `application/Makefile`:

```make
CONSOLE_OPTION +=			\
		-ULOGIN_PRINT_EN	\
		-USYS_PRINT_EN		\
		...
```

Commands still run, but print nothing; the shell only echoes what you type. For a debugging
session, change `-ULOGIN_PRINT_EN` and `-USYS_PRINT_EN` to `-D`, rebuild and flash, then restore `-U`
afterwards. Keep `LOG_AK_KERNEL_ENABLE` in `application/sources/ak/ak.cfg.mk` off unless you are
profiling: it prints every message and starves the shell.

### Documentation tools

| Script | Output |
|---|---|
| `tools/capture_screens.py` | Framebuffer captures of every screen (`resources/captures/`) |
| `tools/capture_demo.py` | The README demo recording |
| `tools/render_screens.py`, `tools/render_demo.py` | Screen PNG/GIF previews and the demo GIF |
| `tools/render_banner.py`, `tools/render_readme_cards.py` | Banner, framed cards, objects table images |
| `tools/render_clock_sequence.py` | README sequence figure |

The capture scripts need the diagnostic build and restore the kit afterwards (sample alarms and
events deleted, RTC re-synced).

## VI. Commit message convention

```bash
git commit -m "[ACTION] short description"
```

- `ACTION`: `ADD`, `UPDATE`, `FIX`, `REMOVE`, `DOC`, `MERGE`.
- Lowercase, imperative description, no trailing period, under about 80 characters.
- One topic per commit; name the module, file or signal when it helps.

```bash
git commit -m "[FIX] re-arm alarms by message after rtc time or timezone changes"
git commit -m "[DOC] add banner-themed screen cards and objects table previews"
```

## VII. Document file naming convention

Documents in `docs/` are numbered in reading order: `NN-<type>-<topic>.md`, where `<type>` is
`guide` (how to work on the project) or `design` (how the firmware works).

| File | Content |
|---|---|
| `01-guide-coding-rules.md` | This guide |
| `02-design-sequence-object.md` | Object ownership and message sequences |
| `03-design-sequence-runtime.md` | Runtime flow, scheduling, screen timers |

## VIII. References

- [AK Embedded Base Kit](https://epcb.vn/products/ak-embedded-base-kit-lap-trinh-nhung-vi-dieu-khien-mcu)
- [Zomwar](https://github.com/caotrongphuoc/zomwar-game) — README and document layout reference
- [AGENTS.md](../AGENTS.md) — rules for AI coding agents working on this repository
