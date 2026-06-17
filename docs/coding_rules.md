<h1 align="center">Coding rules and style guide</h1>

Tài liệu này tổng hợp các quy ước đặt tên và format đang áp dụng trong project, kèm hướng dẫn cài và chạy `clang-format` để mọi người viết code thống nhất.

This document collects the naming conventions and formatting rules used across this project, plus the steps to install and run `clang-format` so every contributor produces consistent code.

---

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
