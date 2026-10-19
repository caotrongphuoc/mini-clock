<h1 align="center">Coding rules and style guide</h1>

This document defines the naming conventions, code style, commit message format, and document naming conventions used in Mini Clock, along with instructions for installing and using `clang-format`. The goal is to keep code from every contributor consistent in form and easy to track through search tools, code review, and version control. Examples come from the existing `mc_clock_*` objects and `scr_clock_*` screens.

---

## Table of contents

- [I. Naming conventions](#i-naming-conventions)
  - [1. Folder](#1-folder)
  - [2. Source and header files](#2-source-and-header-files)
  - [3. Header guard](#3-header-guard)
  - [4. Macros and compile-time constants](#4-macros-and-compile-time-constants)
  - [5. Signal (enum values)](#5-signal-enum-values)
  - [6. Task ID](#6-task-id)
  - [7. Data types and typedef](#7-data-types-and-typedef)
  - [8. Functions](#8-functions)
  - [9. Variables](#9-variables)
- [II. Code style (clang-format)](#ii-code-style-clang-format)
- [III. Installing clang-format](#iii-installing-clang-format)
- [IV. Running clang-format](#iv-running-clang-format)
- [V. Commit message convention](#v-commit-message-convention)
- [VI. Document file naming convention](#vi-document-file-naming-convention)
- [VII. References](#vii-references)

---

## I. Naming conventions

The conventions below are drawn directly from the existing source code. Follow them when you extend the clock so that tooling, search, and reviewers all work consistently.

**Case styles used in this document:**

| Style | Description | Example in project | Applied to |
|---|---|---|---|
| `lower_snake_case` | Lowercase letters, words separated by underscore `_` | `total_alarm`, `mc_clock_alarm_handle` | Variables, functions, typedefs, source file names, folder names |
| `UPPER_SNAKE_CASE` | Uppercase letters, words separated by underscore `_` | `ALARM_NUMBER`, `MC_CLOCK_ALARM_SELECT`, `MC_CLOCK_ALARM_ID` | `#define` constants, enum values, signals, task IDs, macros |
| `kebab-case` | Lowercase letters, words separated by hyphen `-` | `01-guide-coding-rules.md` | Documentation file names under `docs/` |

### 1. Folder

Use `lower_snake_case` for folder names. Organize by feature (feature-based), not by file type.

```
application/sources/app/
  mini_clock/             # source code of every clock object (time, alarm, calendar, ...)
  screens/                # source code of every display screen
  task_mc_rtc.cpp         # RTC task, the only writer of the PCF85063A
  ...
application/sources/driver/
  rtc/                    # PCF85063A driver over I2C1
application/sources/platform/stm32l/
  io_cfg.c/.h             # the only place for GPIO, pin and register code
```

The AK kit folders (`ak/` except `ak.cfg.mk`, `boot/`, `sys/`, `common/`, `networks/`, `libraries/` and the rest of `platform/`) are not modified.

### 2. Source and header files

Source and header files always carry a module prefix so you can identify the module right from the file name:

| Prefix | Meaning | Example |
|---|---|---|
| `mc_clock_*` | Object belonging to the clock | `mc_clock_alarm.cpp`, `mc_clock_calendar.h` |
| `scr_clock_*` | Handler of a clock screen | `scr_clock_main.cpp`, `scr_clock_ringing.h` |
| `task_mc_*` | Clock task that owns hardware | `task_mc_rtc.cpp` |

File extensions: `.h` for headers, `.cpp` for implementation (the project is built as C++).

### 3. Header guard

Use the pattern `__<FILE_NAME>_H__`, fully uppercased, matching the file name exactly:

```cpp
#ifndef __MC_CLOCK_ALARM_H__
#define __MC_CLOCK_ALARM_H__
...
#endif //__MC_CLOCK_ALARM_H__
```

### 4. Macros and compile-time constants

Use `UPPER_SNAKE_CASE` for names. Always wrap numeric values in parentheses to avoid errors when the macro gets expanded, and put a `//` comment line above each group.

**Mandatory rule: a macro that belongs to an object or a screen MUST carry that object's or screen's name as a prefix.**

Pattern: `<OBJECT>_<PROPERTY>`. The object always comes first. Reading the macro name tells you immediately which module it belongs to, and grepping by object name returns every constant of that module.

| Constant type | Correct form |
|---|---|
| Count | `ALARM_NUMBER`, `STOPWATCH_LAP_NUMBER`, `CALENDAR_EVENT_NUMBER`, `WORLD_CLOCK_COUNTRY_NUMBER` |
| Time | `RING_REPEAT_S`, `RING_TIMEOUT_S` |
| Limit | `RTC_WRITE_RETRY_MAX`, `TIME_SETTING_YEAR_MIN`, `TIME_SETTING_YEAR_MAX` |
| Screen layout | `MAIN_TIME_FRAME_X`, `MENU_ICON_X`, `SETTING_FRAME_STEP` |
| Bitmap size | `MENU_ICON_W`, `MENU_ICON_H` |

Examples done right:

```cpp
// mc_clock_alarm.h
// Number of alarms in the list (and in the EEPROM bank)
#define ALARM_NUMBER (10)

// mc_clock_ring.h
// Seconds between melody repeats while ringing
#define RING_REPEAT_S (5)

// Seconds before ringing stops by itself
#define RING_TIMEOUT_S (60)

// scr_clock_setting.h
// Frame layout (origin of first frame and vertical step between frames)
#define SETTING_FRAME_X (2)
#define SETTING_FRAME_Y_1 (2)
#define SETTING_FRAME_STEP (12)
```

Enum values of an object follow the same rule (`ALARM_EDIT_HOUR`, `TIMER_EDIT_SEC`, `CALENDAR_MODE_MONTH`, `RING_SOURCE_ALARM`).

**Exception (project level macros):** macros that belong to the whole clock (not tied to any specific object) use the project prefix `MC_CLOCK_*`:

```cpp
#define MC_CLOCK_TIME_TICK_INTERVAL (1000)
#define MC_CLOCK_EEPROM_MAGIC_NUMBER ((uint32_t)0x6D63636B) // "mcck"
#define MC_CLOCK_SETTING_BRIGHT_LEVEL_DEFAULT (2) // HIGH
```

Group related constants in the right module header (`mc_clock_alarm.h` holds alarm constants, `scr_clock_menu.h` holds menu layout, and so on). Never leave magic numbers scattered across `.cpp` files; a constant used by one `.cpp` only may stay at the top of that file, still with its prefix and comment.

### 5. Signal (enum values)

Signals are the **public contract** between tasks. Always use the full prefix; no abbreviations, not in comments, documentation, or sequence diagrams.

| Pattern | Applied to | Example |
|---|---|---|
| `MC_CLOCK_<OBJECT>_<ACTION>` | Clock object signal | `MC_CLOCK_ALARM_SELECT`, `MC_CLOCK_RTC_SET_TIME_REQ` |
| `AC_DISPLAY_<ACTION>` | Display task signal | `AC_DISPLAY_SHOW_RINGING`, `AC_DISPLAY_BUTTON_MODE_PRESSED` |

Declare each task's signal set in `app.h` as its own block, anchored to `MC_CLOCK_DEFINE_SIG` (or `AK_USER_DEFINE_SIG` for the kit tasks). Timer intervals of the block go under `/* define timer */`, in milliseconds:

```cpp
/*****************************************************************************/
/*  Mini clock 'STOPWATCH' task define
 */
/*****************************************************************************/
/* define timer */
#define MC_CLOCK_STOPWATCH_TICK_INTERVAL							(100)
/* define signal */
enum {
	MC_CLOCK_STOPWATCH_TICK = MC_CLOCK_DEFINE_SIG,
	MC_CLOCK_STOPWATCH_SETUP,
	MC_CLOCK_STOPWATCH_START_PAUSE,
	MC_CLOCK_STOPWATCH_RESET,
	MC_CLOCK_STOPWATCH_UPDATE,
	MC_CLOCK_STOPWATCH_LAP,
	MC_CLOCK_STOPWATCH_HELP,
};
```

### 6. Task ID

Use the pattern `<PREFIX>_<NAME>_ID`, fully uppercased, registered in `task_list.h`:

```cpp
AC_TASK_DISPLAY_ID
MC_CLOCK_RTC_ID
MC_CLOCK_ALARM_ID
```

The corresponding handler in `task_list.cpp` keeps the same name, replacing the `_ID` suffix with `_handle` and lowercased:

```cpp
{MC_CLOCK_ALARM_ID, TASK_PRI_LEVEL_4, mc_clock_alarm_handle},
```

A task that owns hardware uses the kit's `task_` form instead (`MC_CLOCK_RTC_ID` with `task_mc_rtc`). Keep the enum order in `task_list.h` identical to `app_task_table[]`.

### 7. Data types and typedef

Use `lower_snake_case` with the `mc_clock_<object>_` prefix and the `_t` suffix. The struct stays anonymous; the typedef is the public name:

```cpp
typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t enabled;
} mc_clock_alarm_item_t;
```

A message payload is named after the request it carries (`mc_clock_rtc_set_time_req_t`, `mc_clock_ring_req_t`). Types provided by the framework follow the same pattern (`ak_msg_t`, `view_screen_t`).

### 8. Functions

Use `lower_snake_case` with the module name as prefix, so that grepping the prefix returns every entry point of that module:

```cpp
void mc_clock_alarm_handle(ak_msg_t* msg);
void mc_clock_alarm_get_state(mc_clock_alarm_state_t* state);
void mc_clock_ring_request(uint8_t source, uint8_t sound);
uint8_t mc_clock_date_weekday(uint16_t year, uint8_t month, uint8_t day);
```

Helpers used inside one `.cpp` only are `static` and keep the same prefix (`mc_clock_alarm_apply_rtc`, `mc_clock_calendar_shift_month`).

### 9. Variables

Use `lower_snake_case`. Do not start names with an underscore.

- **Object state:** one `static` variable in the object's `.cpp`, changed only by that object's handler. Screens read a copy through `mc_clock_<object>_get_state()`.

  ```cpp
  // mc_clock_alarm.cpp
  static mc_clock_alarm_state_t mc_clock_alarm_state;
  ```

- **Constant tables:** `static const` so they stay in flash (the world clock city table, the menu bitmaps).

- **Globals shared between modules:** declare `extern` in the header, define exactly once in the `.cpp` of the owning module.

  ```cpp
  // mc_clock_calendar.h
  extern const char* const mc_clock_calendar_category_name[CALENDAR_CATEGORY_MAX];
  ```

- **Local variables:** short, describe the role accurately. Loop counters can use `i`, `j`, `k` when the scope is clear. Keep large locals out of handlers: about 3 KB of stack is left.

---

## II. Code style (clang-format)

The repo already includes a `.clang-format` file at the root, shown here for reference:

```yaml
Language: Cpp
BasedOnStyle: LLVM
UseTab: ForIndentation
IndentWidth: 4
TabWidth: 4
ColumnLimit: 0
BreakBeforeBraces: Allman
AllowShortIfStatementsOnASingleLine: false
AllowShortFunctionsOnASingleLine: None
AllowShortBlocksOnASingleLine: false
AllowShortCaseLabelsOnASingleLine: false
PointerAlignment: Left
SpaceBeforeParens: ControlStatements
IndentCaseLabels: false
SortIncludes: false
```

What the non-default settings do:

| Setting | Effect |
|---|---|
| `UseTab: ForIndentation`, `IndentWidth: 4`, `TabWidth: 4` | Tabs are used only for indentation, not alignment. One tab equals 4 columns. |
| `ColumnLimit: 0` | No automatic line wrapping. The author decides where to break a line. |
| `BreakBeforeBraces: Allman` | The `{` always sits on its own line, applies to `if`, `for`, and function bodies. |
| `AllowShort*OnASingleLine: false` | One statement per line, even for short `if` statements or empty functions. |
| `PointerAlignment: Left` | Write `int* p`, not `int *p`. |
| `SpaceBeforeParens: ControlStatements` | `if (x)`, `for (...)` have a space; `func(x)` does not. |
| `IndentCaseLabels: false` | `case` labels sit at the same indent level as `switch`. |
| `SortIncludes: false` | Include order is meaningful (BSP -> framework -> project) and must not be sorted automatically. |

`app.h`, `task_list.cpp` and the other AK kit files keep the kit's own layout (column-aligned tables and `enum {` blocks). Do not run `clang-format` on them.

---

## III. Installing clang-format

Instructions for Linux (Ubuntu / Debian).

Install:

```bash
sudo apt update
sudo apt install clang-format
```

Verify the install:

```bash
clang-format --version
```

Expected output (version number may differ):

```
Ubuntu clang-format version 14.x.x
```

---

## IV. Running clang-format

### 1. Running from the command line

Format a single file in place:

```bash
clang-format -i application/sources/app/mini_clock/mc_clock_alarm.cpp
```

Check a file without changing it (prints a warning for every line that would change):

```bash
clang-format --dry-run application/sources/app/mini_clock/mc_clock_alarm.cpp
```

Format every clock object and clock screen:

```bash
find application/sources/app/mini_clock application/sources/app/screens \
    -type f \( -name "mc_clock_*" -o -name "scr_clock_*" \) \
    -exec clang-format -i {} +
```

Format only the files you change. Reformatting untouched files in a feature commit hides the real change.

### 2. VSCode integration

**Step 1.** Install the **C/C++** extension (Microsoft). It ships with `clang-format` and automatically picks up the `.clang-format` file in the repo.

**Step 2.** Open the workspace settings (`.vscode/settings.json`) and add the following config:

```json
{
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "ms-vscode.cpptools",
    "C_Cpp.clang_format_style": "file",
    "C_Cpp.clang_format_fallbackStyle": "LLVM"
}
```

**Step 3.** Format the current file using the shortcut <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>I</kbd> (Windows / Linux).

**Step 4.** Turn on `Format On Save` so the editor formats the file on every save, making sure no code with stale formatting gets committed. Turn it off, or use a folder-level setting, before editing the kit files listed in [II](#ii-code-style-clang-format).

---

## V. Commit message convention

Every commit must follow the format `[ACTION] short description` so the git history stays easy to read and easy to filter.

### 1. Workflow

```bash
git add <path>                                # stage the files of one topic
git commit -m "[ACTION] short description"    # tag is mandatory, keep description short
git push                                      # push to remote
```

Stage specific paths instead of `git add .` to avoid committing build output or junk files by mistake.

### 2. Action tags

| Tag | When to use |
|---|---|
| `[ADD]` | Adding a new file, feature, asset, or document |
| `[UPDATE]` | Updating existing code (refactor, rename, tweak logic, bump version) |
| `[FIX]` | Fixing an existing bug, build error, or formatting error |
| `[REMOVE]` | Removing a file, feature, or dead code |
| `[DOC]` | Documentation-only changes (`docs/`, `README.md`, large comment blocks) |
| `[MERGE]` | Branch merges (usually tool-generated, do not hand-edit) |

### 3. Description style

- Tag fully uppercased inside `[]`, followed by exactly one space, then the description.
- Description in lowercase, imperative mood (`add`, `fix`, `rename`, `move`...), no trailing period.
- Keep the length around 70 characters. If longer, shorten it or move the details into the commit body.
- When the change touches a specific module / signal / file, name it directly so the history is easy to grep.

### 4. Good examples

```text
[ADD] poll rtc alarm flag and show ringing screen with dismiss
[ADD] persist calendar events via eeprom
[UPDATE] run timer, stopwatch and calendar alarms off-screen with shared ringing
[FIX] re-arm alarms by message after rtc time or timezone changes
[FIX] move world clock country table to flash to stop stack overflow
[REMOVE] remove ak-docs mcp config and stale gitignore entry
[DOC] add object and runtime sequence docs and clock sequence figure
```

### 5. Examples to avoid

```text
update                          # missing tag
[update] fix something          # tag must be uppercase
[ADD] Added new file.           # no past tense and no trailing period
[FIX] fix bug                   # too vague, which bug?
[ADD] mc_clock_alarm.cpp + mc_clock_calendar.cpp + scr_clock_main.cpp ... # too long, group by topic
```

---

## VI. Document file naming convention

Files in `docs/` follow the format `<NN>-<category>-<topic>.md`:

| Component | Convention | Example |
|---|---|---|
| `NN` | A 2-digit sequence number, starting from `01`. Reflects reading order: guides come first, design docs come after. | `01`, `02`, `03` |
| `category` | Document category. Only use predefined values; do not add new categories. | `guide`, `design` |
| `topic` | Main topic, written in `kebab-case` (lowercase, words separated by `-`). | `coding-rules`, `sequence-object` |

Categories currently in use:

| Category | Purpose | Typical content |
|---|---|---|
| `guide` | Workflow and process guides for contributors | Coding rules |
| `design` | Describes architecture and runtime behavior of the system | Sequence diagrams |

Files currently in the repo:

```
docs/
├── 01-guide-coding-rules.md
├── 02-design-sequence-object.md
└── 03-design-sequence-runtime.md
```

A few important notes:

- Documentation files (`.md`) use `kebab-case` (hyphens). Source files and folders use `snake_case` (underscores). The difference is intentional: `kebab-case` is the standard convention for Markdown slugs and URLs.
- Images that go with a document live under `resources/images/<topic>/`, named after the document topic. For example: `resources/images/design-sequence/`.
- When adding a new document, continue the sequence number from the current maximum and keep the same category together to preserve a natural reading order.
- Renaming a documentation file must be done with `git mv` so that the rename history is tracked properly.

---

## VII. References

- [Naming convention: Multi-word identifiers (Wikipedia)](https://en.wikipedia.org/wiki/Naming_convention_(programming)#Multiple-word_identifiers): definitions of `snake_case`, `SCREAMING_SNAKE_CASE`, and `kebab-case` used in Sections I and VI.
- [Clang-Format: Configurable Format Style Options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#configurable-format-style-options): describes every key in the `.clang-format` file from Section II.
- [LLVM Coding Standards: Source Code Formatting](https://llvm.org/docs/CodingStandards.html#source-code-formatting): base style inherited via `BasedOnStyle: LLVM` in Section II.
- [Pro Git: Recording Changes to the Repository](https://git-scm.com/book/en/v2/Git-Basics-Recording-Changes-to-the-Repository): the `git add` / `git commit` / `git push` workflow used in Section V.

---

## Contact & Support

<p style="font-size: 20px;"><strong>Cao Trong Phuoc</strong> - Software Engineer - Embedded Systems</p>

```text
Thank you for visiting this repository.
If you have any questions, suggestions, or feedback about this project or firmware development, feel free to contact me directly.
```

<a href="https://github.com/caotrongphuoc">
  <img src="https://img.shields.io/badge/GitHub-caotrongphuoc-181717?style=for-the-badge&logo=github&logoColor=white" alt="GitHub"/>
</a>

<a href="https://www.linkedin.com/in/cao-trong-phuoc/">
  <img src="https://img.shields.io/badge/LinkedIn-Cao%20Trong%20Phuoc-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white" alt="LinkedIn"/>
</a>

<p style="font-size: 20px;"><strong>Josef Tan</strong> - Software Engineer - Embedded Systems</p>

```text
For questions related to this project, embedded systems, or firmware development, please feel free to reach out through GitHub or LinkedIn.
```

<a href="https://github.com/joseftanlang">
  <img src="https://img.shields.io/badge/GitHub-joseftanlang-181717?style=for-the-badge&logo=github&logoColor=white" alt="GitHub"/>
</a>

<a href="https://www.linkedin.com/in/josef-tan/">
  <img src="https://img.shields.io/badge/LinkedIn-Josef%20Tan-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white" alt="LinkedIn"/>
</a>
