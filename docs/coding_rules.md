<h1 align="center">Coding rules and style guide</h1>

Tài liệu này tổng hợp các quy ước đặt tên và format đang áp dụng trong project, kèm hướng dẫn cài và chạy `clang-format` để mọi người viết code thống nhất.

This document collects the naming conventions and formatting rules used across this project, plus the steps to install and run `clang-format` so every contributor produces consistent code.

---

# Tiếng Việt

## Mục lục

- [I. Quy ước đặt tên](#i-quy-ước-đặt-tên)
  - [1. Folder](#1-folder)
  - [2. File source và header](#2-file-source-và-header)
  - [3. Header guard](#3-header-guard)
  - [4. Macro và hằng số biên dịch](#4-macro-và-hằng-số-biên-dịch)
  - [5. Signal (giá trị enum)](#5-signal-giá-trị-enum)
  - [6. Task ID](#6-task-id)
  - [7. Kiểu dữ liệu và typedef](#7-kiểu-dữ-liệu-và-typedef)
  - [8. Hàm](#8-hàm)
  - [9. Biến](#9-biến)
- [II. Code style (clang-format)](#ii-code-style-clang-format)
- [III. Cài đặt clang-format](#iii-cài-đặt-clang-format)
- [IV. Chạy clang-format](#iv-chạy-clang-format)
- [V. Quy ước commit message](#v-quy-ước-commit-message)
- [VI. Tuỳ chọn: pre-commit hook](#vi-tuỳ-chọn-pre-commit-hook)

---

## I. Quy ước đặt tên

Các quy ước dưới đây không phải đặt ra ngẫu nhiên — chúng được rút ra từ chính code hiện có. Code mới nên tiếp tục theo đúng convention để tooling, search và reviewer làm việc trơn tru.

### 1. Folder

Viết thường, dùng dấu gạch dưới. Gom theo tính năng, không theo đuôi file.

```
application/sources/app/
  game/
    game_zomwar/        # mỗi game một folder
  screens/              # mỗi nhóm screen một folder
  ...
```

### 2. File source và header

File luôn có **prefix module** để vừa nhìn là biết code nằm ở mảng nào:

| Prefix | Ý nghĩa | Ví dụ |
|---|---|---|
| `app_*` | Glue tầng application | `app_bsp.cpp`, `app_eeprom.h` |
| `task_*` | Entry point task AK framework | `task_display.cpp`, `task_life.h` |
| `scr_*` | Handler của một screen | `scr_game_zomwar.cpp`, `scr_game_over.h` |
| `zw_game_*` | Một object của game Zomwar | `zw_game_bang.cpp`, `zw_game_border.h` |

Khi thêm game mới, chọn prefix ngắn riêng (ví dụ `ar_game_*` cho game bắn cung) rồi áp dụng đồng đều cho mọi file trong folder.

Đuôi file: `.h` cho header, `.cpp` cho implementation (project biên dịch C++).

### 3. Header guard

Dùng `__<TÊN_FILE>_H__`, viết hoa hết, khớp tên file:

```cpp
#ifndef __ZW_GAME_BANG_H__
#define __ZW_GAME_BANG_H__
...
#endif //__ZW_GAME_BANG_H__
```

### 4. Macro và hằng số biên dịch

`UPPER_SNAKE_CASE`. Luôn bọc số trong dấu ngoặc để khỏi sai khi macro được expand:

```cpp
#define NUM_BULLET                 (15)
#define AXIS_X_GUNNER              (14)
#define WAVE_SCORE_INTERVAL        (200)
#define ZW_GAME_TIME_TICK_INTERVAL (100)
```

Gom hằng số liên quan vào header tương ứng (`zw_game_bullet.h` chứa hằng của bullet, v.v.). Tránh rải magic number rải rác trong `.cpp`.

### 5. Signal (giá trị enum)

Signal là **hợp đồng công khai** giữa các task. Luôn dùng full prefix — không bao giờ viết tắt, kể cả trong comment hay sequence diagram.

| Prefix | Dùng cho | Ví dụ |
|---|---|---|
| `AK_*` | Core AK framework | `AK_USER_DEFINE_SIG` |
| `AC_<SUBSYSTEM>_*` | Các subsystem AK chung | `AC_DISPLAY_BUTTON_UP_PRESSED` |
| `<GAME>_<OBJECT>_<ACTION>` | Signal theo game | `ZW_GAME_BORDER_CHECK_GAME_OVER` |

Khai báo signal set của mỗi task trong `app.h` thành một enum block riêng, anchor vào `ZW_GAME_DEFINE_SIG` (hoặc `AK_USER_DEFINE_SIG` cho nhóm framework):

```cpp
/*****************************************************************************/
/*  Zomwar game 'BORDER' task define
 */
/*****************************************************************************/
enum {
    ZW_GAME_BORDER_SETUP = ZW_GAME_DEFINE_SIG,
    ZW_GAME_BORDER_CHECK_GAME_OVER,
    ZW_GAME_BORDER_CHECK_WAVE,
    ZW_GAME_BORDER_LEVEL_UP,
    ZW_GAME_BORDER_RESET,
};
```

### 6. Task ID

`<PREFIX>_<TÊN>_ID`, viết hoa hết, đăng ký trong `task_list.h`:

```cpp
AC_TASK_DISPLAY_ID
ZW_GAME_GUNNER_ID
ZW_GAME_BORDER_ID
```

Handler tương ứng trong `task_list.cpp` giữ nguyên tên trừ hậu tố `_ID` rồi thêm `_handle`:

```cpp
{ZW_GAME_BORDER_ID, TASK_PRI_LEVEL_4, zw_game_border_handle},
```

### 7. Kiểu dữ liệu và typedef

`lower_snake_case` cộng hậu tố `_t`. Bản thân struct vô danh; typedef là tên public:

```cpp
typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t action_image;
    bool    visible;
} zw_game_gunner_t;
```

Kiểu do framework cung cấp cũng theo pattern này (`ak_msg_t`, `view_screen_t`).

### 8. Hàm

`lower_snake_case` kèm prefix **tên module**, để `grep` prefix là ra hết entry point của module đó:

```cpp
void zw_game_border_handle(ak_msg_t* msg);
void zw_game_bang_spawn(int16_t x, uint8_t y);
void zw_game_zombie_spawn_from_tombstone(uint8_t i, int16_t x, uint8_t y);
int8_t zw_game_car_find_nearest(uint8_t zy);
```

Helper private của module để `static` trong `.cpp` và giữ nguyên prefix:

```cpp
static void zw_game_car_reset_slot(uint8_t i);
```

### 9. Biến

`lower_snake_case`. Không Hungarian notation, không bắt đầu bằng dấu gạch dưới.

- **Biến global chia sẻ giữa các module**: khai báo `extern` trong header, định nghĩa đúng một lần trong `.cpp` của module sở hữu.
  ```cpp
  // zw_game_border.h
  extern uint16_t zw_game_score;
  extern uint8_t  wave_level;
  extern bool     wave_warning_active;
  ```
- **Biến private của module**: khai báo `static` trong `.cpp`.
  ```cpp
  // scr_game_zomwar.cpp
  static uint8_t zw_game_state;
  static uint8_t gunner_dir = GUNNER_DIR_NONE;
  ```
- **Biến local**: ngắn và mô tả được ý. Biến đếm vòng lặp có thể dùng `i`, `j`, `l` khi scope rõ ràng.

State của object thuộc task nào thì gắn tên object đó (`gunner.y`, `bang[i].visible`, `wave_warning_timer`) — không nhét state cross-cutting vào `.cpp` module khác.

---

## II. Code style (clang-format)

Repo đã có sẵn `.clang-format` ở thư mục gốc. Mọi contributor phải chạy `clang-format` trước khi commit — config check-in trong repo, không phải tùy chỉnh per-developer:

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

Ý nghĩa từng setting khác mặc định:

| Setting | Tác dụng |
|---|---|
| `UseTab: ForIndentation`, `IndentWidth: 4`, `TabWidth: 4` | Tab chỉ dùng để indent, không dùng để align. Một tab = 4 cột. |
| `ColumnLimit: 0` | Không tự động xuống dòng. Tác giả tự quyết định break dòng ở đâu. |
| `BreakBeforeBraces: Allman` | Dấu `{` luôn ở dòng riêng — kể cả `if`, `for`, function body. |
| `AllowShort*OnASingleLine: false` | Mỗi statement một dòng, dù `if` ngắn hay function rỗng. |
| `PointerAlignment: Left` | Viết `int* p`, không phải `int *p`. |
| `SpaceBeforeParens: ControlStatements` | `if (x)`, `for (...)` có space; `func(x)` không có. |
| `IndentCaseLabels: false` | `case` cùng cấp indent với `switch`. |
| `SortIncludes: false` | Thứ tự include là có ý nghĩa (BSP → framework → project), không được sort. |

Nếu phải "đánh nhau" với formatter, giải pháp là cấu trúc lại code, không phải tắt rule.

---

## III. Cài đặt clang-format

Hướng dẫn cho Linux (Ubuntu / Debian). Cần phiên bản **≥ 14** để support đầy đủ các setting.

Cài bản trong apt repo của distro:

```bash
sudo apt update
sudo apt install clang-format
```

Muốn bản mới hơn (LLVM apt repo):

```bash
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18   # cài LLVM 18 toolchain, gồm clang-format-18
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100
```

Kiểm tra cài đặt:

```bash
clang-format --version
```

Kết quả mong đợi (số version có thể khác):

```
Ubuntu clang-format version 18.1.x
```

---

## IV. Chạy clang-format

### Command line

Format 1 file in-place:

```bash
clang-format -i application/sources/app/game/game_zomwar/zw_game_bullet.cpp
```

Format toàn bộ C / C++ source và header trong `application/sources/app`:

```bash
find application/sources/app -type f \( -name "*.cpp" -o -name "*.h" \) \
    -not -path "*/libraries/*" \
    -exec clang-format -i {} +
```

Dry-run (chỉ in diff, không ghi) — chạy trước khi commit để check:

```bash
clang-format --dry-run --Werror path/to/file.cpp
```

### VSCode integration

1. Cài extension **C/C++** (Microsoft) — extension này tự kèm `clang-format` và tôn trọng `.clang-format` trong repo.
2. Mở workspace settings (`.vscode/settings.json`), thêm:

   ```json
   {
       "editor.formatOnSave": true,
       "editor.defaultFormatter": "ms-vscode.cpptools",
       "C_Cpp.clang_format_style": "file",
       "C_Cpp.clang_format_fallbackStyle": "LLVM"
   }
   ```

3. Format file hiện tại theo phím tắt: <kbd>Shift</kbd>+<kbd>Alt</kbd>+<kbd>F</kbd> (Windows / Linux) hoặc <kbd>Shift</kbd>+<kbd>Option</kbd>+<kbd>F</kbd> (macOS).

Bật `formatOnSave` thì mỗi lần save editor sẽ format ngay, không bao giờ commit code lệch format.

---

## V. Quy ước commit message

Mọi commit phải theo format `[ACTION] mô tả ngắn` để lịch sử git dễ đọc và filter.

### Workflow

```bash
git add .                                     # stage toàn bộ thay đổi
git commit -m "[ACTION] mô tả ngắn"           # tag bắt buộc, mô tả ngắn gọn
git push                                      # đẩy lên remote
```

Nếu chỉ muốn stage một số file cụ thể, thay `git add .` bằng `git add <path>` để tránh lỡ tay commit file rác.

### Action tag

| Tag | Khi nào dùng |
|---|---|
| `[ADD]` | Thêm file, feature, asset, doc mới |
| `[UPDATE]` | Cập nhật code có sẵn — refactor, đổi tên, tinh chỉnh logic, nâng version |
| `[FIX]` | Sửa bug đã có hoặc lỗi build / lỗi format |
| `[REMOVE]` | Xoá file, feature, hoặc code chết |
| `[DOC]` | Chỉ đụng tới tài liệu (`docs/`, `README.md`, comment block lớn) |
| `[MERGE]` | Merge branch (thường do tool tự sinh, không sửa tay) |

### Style mô tả

- Tag viết hoa hết trong `[]`, theo sau đúng một dấu cách rồi mới đến mô tả.
- Mô tả viết thường, dạng mệnh lệnh (`add`, `fix`, `rename`, `move`...), không dấu chấm cuối câu.
- Giữ trong ~70 ký tự — nếu dài hơn thì cắt ngắn, hoặc tách body commit ở dòng dưới.
- Khi đụng tới module / signal / file cụ thể, gọi tên trực tiếp giúp grep lịch sử dễ hơn.

### Ví dụ tốt

```text
[ADD] tombstone object sequence
[ADD] mermaid border sequence diagram
[UPDATE] rename Border signals to ZW_GAME_BORDER_*
[UPDATE] split Bang narrative into Setup/Per-tick/Reset
[UPDATE] enlarge runtime mermaid font size
[FIX] off-by-one in zombie zigzag clamp
[REMOVE] unused gunner_dir static in scr_idle
[DOC] coding rules and clang-format setup guide
```

### Ví dụ cần tránh

```text
update                          # thiếu tag
[update] fix something          # tag phải viết hoa
[ADD] Added new file.           # đừng dùng động từ thì quá khứ + chấm câu
[FIX] fix bug                   # quá mơ hồ, không biết bug gì
[ADD] zw_game_border.cpp + zw_game_zombie.cpp + scr_game_zomwar.cpp ... # quá dài, nên gộp chủ đề
```

---

## VI. Tuỳ chọn: pre-commit hook

Một Git hook nhẹ có thể chặn commit chứa file chưa format. Lưu file dưới đây thành `.git/hooks/pre-commit` rồi `chmod +x .git/hooks/pre-commit`:

```bash
#!/usr/bin/env bash
set -e
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|cpp|h|hpp)$' | grep -v 'libraries/' || true)
if [ -z "$files" ]; then
    exit 0
fi
fail=0
for f in $files; do
    if ! diff -q <(clang-format "$f") "$f" > /dev/null; then
        echo "clang-format: $f is not formatted"
        fail=1
    fi
done
if [ "$fail" -ne 0 ]; then
    echo
    echo "Run: clang-format -i <file>   (hoặc bật Format On Save trong editor)"
    exit 1
fi
```

Hook này là opt-in — Git hook không được commit mặc định. Share snippet này qua tài liệu và để mỗi người tự cài.

---

# English

## Table of Contents

- [I. Naming Conventions](#i-naming-conventions)
  - [1. Folders](#1-folders)
  - [2. Source and header files](#2-source-and-header-files)
  - [3. Header guards](#3-header-guards)
  - [4. Macros and compile-time constants](#4-macros-and-compile-time-constants)
  - [5. Signals (enum values)](#5-signals-enum-values)
  - [6. Task IDs](#6-task-ids)
  - [7. Types and typedefs](#7-types-and-typedefs)
  - [8. Functions](#8-functions)
  - [9. Variables](#9-variables)
- [II. Code Style (clang-format)](#ii-code-style-clang-format)
- [III. Installing clang-format](#iii-installing-clang-format)
- [IV. Running clang-format](#iv-running-clang-format)
- [V. Commit message convention](#v-commit-message-convention)
- [VI. Optional: pre-commit hook](#vi-optional-pre-commit-hook)

---

## I. Naming Conventions

The rules below are not arbitrary — they come from patterns already present in the codebase. New code should keep following them so existing tooling, search, and reviewers stay productive.

### 1. Folders

Lowercase with underscores. Group by feature, not by file extension.

```
application/sources/app/
  game/
    game_zomwar/        # one folder per game
  screens/              # one folder per screen family
  ...
```

### 2. Source and header files

Files use a **module prefix** that tells you immediately where the code lives:

| Prefix | Meaning | Example |
|---|---|---|
| `app_*` | Application-level glue | `app_bsp.cpp`, `app_eeprom.h` |
| `task_*` | AK framework task entry points | `task_display.cpp`, `task_life.h` |
| `scr_*` | A screen handler | `scr_game_zomwar.cpp`, `scr_game_over.h` |
| `zw_game_*` | A Zomwar game object | `zw_game_bang.cpp`, `zw_game_border.h` |

When adding a new game, pick its own short prefix (for example `ar_game_*` for an archery game) and apply it consistently to every file in that folder.

File extensions: `.h` for headers, `.cpp` for implementations (the project compiles as C++).

### 3. Header guards

Use `__<FILE_NAME>_H__`, all uppercase, matching the file name:

```cpp
#ifndef __ZW_GAME_BANG_H__
#define __ZW_GAME_BANG_H__
...
#endif //__ZW_GAME_BANG_H__
```

### 4. Macros and compile-time constants

`UPPER_SNAKE_CASE`. Always wrap numeric constants in parentheses so they survive expansion:

```cpp
#define NUM_BULLET                 (15)
#define AXIS_X_GUNNER              (14)
#define WAVE_SCORE_INTERVAL        (200)
#define ZW_GAME_TIME_TICK_INTERVAL (100)
```

Group related constants together in the corresponding header (`zw_game_bullet.h` holds bullet constants, etc.). Avoid scattering magic numbers in `.cpp` files.

### 5. Signals (enum values)

Signals are the **public contract** between tasks. Always use the full prefix — never abbreviate, even in comments or diagrams.

| Prefix | Used by | Example |
|---|---|---|
| `AK_*` | AK framework core | `AK_USER_DEFINE_SIG` |
| `AC_<SUBSYSTEM>_*` | Common AK subsystems | `AC_DISPLAY_BUTTON_UP_PRESSED` |
| `<GAME>_<OBJECT>_<ACTION>` | Per-game signals | `ZW_GAME_BORDER_CHECK_GAME_OVER` |

Define each task's signal set in `app.h` as its own enum block, anchored at `ZW_GAME_DEFINE_SIG` (or `AK_USER_DEFINE_SIG` for framework-level groups):

```cpp
/*****************************************************************************/
/*  Zomwar game 'BORDER' task define
 */
/*****************************************************************************/
enum {
    ZW_GAME_BORDER_SETUP = ZW_GAME_DEFINE_SIG,
    ZW_GAME_BORDER_CHECK_GAME_OVER,
    ZW_GAME_BORDER_CHECK_WAVE,
    ZW_GAME_BORDER_LEVEL_UP,
    ZW_GAME_BORDER_RESET,
};
```

### 6. Task IDs

`<PREFIX>_<NAME>_ID`, all uppercase, registered in `task_list.h`:

```cpp
AC_TASK_DISPLAY_ID
ZW_GAME_GUNNER_ID
ZW_GAME_BORDER_ID
```

The matching handler in `task_list.cpp` keeps the same name minus the `_ID` suffix plus `_handle`:

```cpp
{ZW_GAME_BORDER_ID, TASK_PRI_LEVEL_4, zw_game_border_handle},
```

### 7. Types and typedefs

`lower_snake_case` with a `_t` suffix. The struct itself is anonymous; the typedef is the public name:

```cpp
typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t action_image;
    bool    visible;
} zw_game_gunner_t;
```

Framework-supplied types follow the same pattern (`ak_msg_t`, `view_screen_t`).

### 8. Functions

`lower_snake_case` prefixed with the **module name**, so a `grep` for the prefix gives every entry point of that module:

```cpp
void zw_game_border_handle(ak_msg_t* msg);
void zw_game_bang_spawn(int16_t x, uint8_t y);
void zw_game_zombie_spawn_from_tombstone(uint8_t i, int16_t x, uint8_t y);
int8_t zw_game_car_find_nearest(uint8_t zy);
```

Module-private helpers go `static` inside the `.cpp` and keep the same prefix:

```cpp
static void zw_game_car_reset_slot(uint8_t i);
```

### 9. Variables

`lower_snake_case`. No Hungarian notation, no leading underscores.

- **Globals exposed across modules**: declared in the header with `extern`, defined once in the `.cpp` that owns them.
  ```cpp
  // zw_game_border.h
  extern uint16_t zw_game_score;
  extern uint8_t  wave_level;
  extern bool     wave_warning_active;
  ```
- **Module-private state**: declared `static` in the `.cpp`.
  ```cpp
  // scr_game_zomwar.cpp
  static uint8_t zw_game_state;
  static uint8_t gunner_dir = GUNNER_DIR_NONE;
  ```
- **Locals**: short and descriptive. Loop indices may be `i`, `j`, `l` when scope is obvious.

Game state owned by an object task carries the **object name** (`gunner.y`, `bang[i].visible`, `wave_warning_timer`) — never put cross-cutting state into another module's `.cpp`.

---

## II. Code Style (clang-format)

The repository ships a `.clang-format` at the root. Every contributor must run `clang-format` before committing — the settings are not negotiable per-developer, they are checked in:

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

What each non-default setting buys you:

| Setting | Effect |
|---|---|
| `UseTab: ForIndentation`, `IndentWidth: 4`, `TabWidth: 4` | Tabs only for indentation, never for alignment. One tab = 4 visual columns. |
| `ColumnLimit: 0` | No automatic line wrapping. The author decides where lines break. |
| `BreakBeforeBraces: Allman` | Opening brace always on its own line — even for `if`, `for`, function bodies. |
| `AllowShort*OnASingleLine: false` | One statement per line, even for short `if` or empty function bodies. |
| `PointerAlignment: Left` | Write `int* p`, not `int *p`. |
| `SpaceBeforeParens: ControlStatements` | `if (x)` and `for (...)`, but `func(x)`. |
| `IndentCaseLabels: false` | `case` labels sit at the same indent as `switch`. |
| `SortIncludes: false` | Include order is meaningful (BSP, framework, project) — don't shuffle it. |

If you find yourself fighting the formatter, the answer is to restructure the code, not to disable the rule.

---

## III. Installing clang-format

Instructions for Linux (Ubuntu / Debian). You need version **≥ 14** for full support of the settings used here.

Install from the distro apt repo:

```bash
sudo apt update
sudo apt install clang-format
```

For a newer version (LLVM apt repos):

```bash
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18   # picks LLVM 18 toolchain, includes clang-format-18
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100
```

Verify the installation:

```bash
clang-format --version
```

Expected output (version number may differ):

```
Ubuntu clang-format version 18.1.x
```

---

## IV. Running clang-format

### Command line

Format a single file in place:

```bash
clang-format -i application/sources/app/game/game_zomwar/zw_game_bullet.cpp
```

Format every C / C++ source and header under `application/sources/app`:

```bash
find application/sources/app -type f \( -name "*.cpp" -o -name "*.h" \) \
    -not -path "*/libraries/*" \
    -exec clang-format -i {} +
```

Dry-run (show diffs without writing) — useful before committing:

```bash
clang-format --dry-run --Werror path/to/file.cpp
```

### VSCode integration

1. Install the **C/C++** extension (Microsoft) — it ships its own `clang-format` and respects the repo's `.clang-format`.
2. Open the workspace settings (`.vscode/settings.json`) and add:

   ```json
   {
       "editor.formatOnSave": true,
       "editor.defaultFormatter": "ms-vscode.cpptools",
       "C_Cpp.clang_format_style": "file",
       "C_Cpp.clang_format_fallbackStyle": "LLVM"
   }
   ```

3. Format the current file on demand: <kbd>Shift</kbd>+<kbd>Alt</kbd>+<kbd>F</kbd> (Windows / Linux) or <kbd>Shift</kbd>+<kbd>Option</kbd>+<kbd>F</kbd> (macOS).

With `formatOnSave` enabled the editor reformats every file the moment you save it, so you never push unformatted code.

---

## V. Commit message convention

Every commit must follow `[ACTION] short description` so the git history stays readable and filterable.

### Workflow

```bash
git add .                                     # stage all changes
git commit -m "[ACTION] short description"    # tag is mandatory, keep description terse
git push                                      # push to remote
```

If you only want to stage specific files, swap `git add .` for `git add <path>` so you don't sweep in junk by accident.

### Action tags

| Tag | When to use |
|---|---|
| `[ADD]` | New file, feature, asset, or doc |
| `[UPDATE]` | Modify existing code — refactor, rename, tweak logic, bump version |
| `[FIX]` | Patch an existing bug or break (build / format failures count) |
| `[REMOVE]` | Delete a file, feature, or dead code |
| `[DOC]` | Documentation only (`docs/`, `README.md`, large comment blocks) |
| `[MERGE]` | Branch merges (usually tool-generated — don't hand-edit) |

### Description style

- Tag is uppercase inside `[]`, followed by a single space before the description.
- Description is lowercase imperative (`add`, `fix`, `rename`, `move`...), no trailing period.
- Keep it under ~70 characters — break into a body paragraph if you need more.
- When touching a specific module / signal / file, name it directly so future `git log | grep` works.

### Good examples

```text
[ADD] tombstone object sequence
[ADD] mermaid border sequence diagram
[UPDATE] rename Border signals to ZW_GAME_BORDER_*
[UPDATE] split Bang narrative into Setup/Per-tick/Reset
[UPDATE] enlarge runtime mermaid font size
[FIX] off-by-one in zombie zigzag clamp
[REMOVE] unused gunner_dir static in scr_idle
[DOC] coding rules and clang-format setup guide
```

### Bad examples

```text
update                          # missing tag
[update] fix something          # tag must be uppercase
[ADD] Added new file.           # past tense and trailing period — don't
[FIX] fix bug                   # too vague, which bug?
[ADD] zw_game_border.cpp + zw_game_zombie.cpp + scr_game_zomwar.cpp ... # too long, group by theme
```

---

## VI. Optional: pre-commit hook

A lightweight Git hook can block commits that contain unformatted files. Save the following as `.git/hooks/pre-commit` and make it executable (`chmod +x .git/hooks/pre-commit`):

```bash
#!/usr/bin/env bash
set -e
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|cpp|h|hpp)$' | grep -v 'libraries/' || true)
if [ -z "$files" ]; then
    exit 0
fi
fail=0
for f in $files; do
    if ! diff -q <(clang-format "$f") "$f" > /dev/null; then
        echo "clang-format: $f is not formatted"
        fail=1
    fi
done
if [ "$fail" -ne 0 ]; then
    echo
    echo "Run: clang-format -i <file>   (or enable Format On Save in your editor)"
    exit 1
fi
```

This is opt-in — Git hooks are not committed by default. Share the snippet through this document and let contributors install it locally.
