<h1 align="center">Coding rules and style guide</h1>

Tài liệu này tổng hợp các quy ước đặt tên và format đang áp dụng trong project, kèm hướng dẫn cài và chạy `clang-format` để mọi người viết code thống nhất.

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

> **Trước khi đọc tiếp — các kiểu viết tên (case style):**
>
> | Kiểu | Cách viết | Ví dụ trong project | Dùng cho |
> |---|---|---|---|
> | `lower_snake_case` | Chữ **thường** hết, các từ nối bằng dấu gạch dưới `_` | `wave_warning_active`, `zw_game_score`, `gunner_y` | Biến, hàm, typedef, tên file, tên folder |
> | `UPPER_SNAKE_CASE` | Chữ **hoa** hết, các từ nối bằng dấu gạch dưới `_` | `NUM_BULLET`, `ZW_GAME_BORDER_SETUP`, `AC_TASK_DISPLAY_ID` | Hằng số `#define`, signal enum, task ID, macro |

### 1. Folder

Viết thường, dùng dấu gạch dưới. Gom theo tính năng, không theo đuôi file.

```
application/sources/app/
  game/
    game_ten_game_cua_ban/        # mỗi game một folder, ví dụ như game_zomwar (lưu ý viết thưởng)
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

Khi code game, chọn prefix ngắn riêng (ví dụ `zw_game_*` cho game bắn zombie) rồi áp dụng đồng đều cho mọi file trong folder.

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

`UPPER_SNAKE_CASE`. Luôn bọc số trong dấu ngoặc để khỏi sai khi macro được expand.

**Quy tắc bắt buộc: macro thuộc về object nào thì PHẢI có tên object đó làm prefix.**

Pattern: `<OBJECT>_<PROPERTY>` hoặc `<OBJECT>_<ACTION>` — object luôn đứng trước, không bao giờ đứng sau. Đọc tên macro là đoán được nó thuộc module nào, grep theo object là ra toàn bộ hằng số của nó.

| Loại hằng | Đặt đúng | Tránh |
|---|---|---|
| Số lượng | `ZOMBIE_NUMBER`, `BULLET_NUMBER`, `BANG_NUMBER` | ~~`NUM_ZOMBIE`~~, ~~`NUM_BULLET`~~ |
| Tọa độ | `GUNNER_AXIS_X`, `CAR_AXIS_X` | ~~`AXIS_X_GUNNER`~~, ~~`AXIS_X_CAR`~~ |
| Bước di chuyển | `GUNNER_STEP_AXIS_Y`, `BULLET_STEP_AXIS_X` | ~~`STEP_GUNNER_AXIS_Y`~~ |
| Kích thước bitmap | `GUNNER_SIZE_BITMAP_X`, `BANG_SIZE_BITMAP_I_X` | ~~`SIZE_BITMAP_GUNNER_X`~~ |
| Thời gian / chu kỳ | `BORDER_WAVE_SCORE_INTERVAL`, `TOMBSTONE_SPAWN_INTERVAL` | ~~`WAVE_SCORE_INTERVAL`~~ |
| Hành vi / giới hạn | `ZOMBIE_RISE_TICKS`, `ZOMBIE_SPEED_MAX`, `CAR_HIT_RANGE_Y` | ~~`RISE_TICKS_ZOMBIE`~~, ~~`MAX_SPEED_ZOMBIE`~~ |

Ví dụ áp dụng đúng:

```cpp
// zw_game_bullet.h
#define BULLET_NUMBER              (15)
#define BULLET_MAX_AXIS_X          (128)
#define BULLET_STEP_AXIS_X         (3)
#define BULLET_SIZE_BITMAP_X       (5)
#define BULLET_SIZE_BITMAP_Y       (5)

// zw_game_gunner.h
#define GUNNER_STEP_AXIS_Y         (10)
#define GUNNER_AXIS_X              (14)
#define GUNNER_AXIS_Y              (52)
#define GUNNER_AXIS_Y_MIN          (12)
#define GUNNER_AXIS_Y_MAX          (52)
#define GUNNER_SIZE_BITMAP_X       (25)
#define GUNNER_SIZE_BITMAP_Y       (10)

// zw_game_border.h
#define BORDER_WAVE_SCORE_INTERVAL    (200)
#define BORDER_WARNING_BLINK_DURATION (30)
#define BORDER_WARNING_BLINK_RATE     (3)
#define BORDER_SIZE_BITMAP_WARNING_X  (16)
#define BORDER_SIZE_BITMAP_WARNING_Y  (14)
```

Tên số nhiều ở mục "số lượng" có thể dùng `NUMBER` hay `NUM` đều được — chọn một và dùng nhất quán trong cùng module.

**Ngoại lệ — system / project-level macro**: macro thuộc về toàn bộ game (không phải của object cụ thể) thì dùng project prefix `ZW_GAME_*`:

```cpp
#define ZW_GAME_TIME_TICK_INTERVAL (100)
#define ZW_GAME_TIME_EXIT_INTERVAL (3000)
```

Gom hằng số liên quan vào header tương ứng (`zw_game_bullet.h` chứa hằng bullet, `zw_game_gunner.h` chứa hằng gunner...). Tránh rải magic number rải rác trong `.cpp`.

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

## Contact & Support

<p style="font-size: 20px;"><strong>Cao Trong Phuoc</strong> - Software Engineer - Embedded Systems</p>

``` Note
Thank you for visiting this repository.
If you have any questions, suggestions, or feedback about this project or firmware development, feel free to contact me directly.
```

<a href="https://github.com/caotrongphuoc">
  <img src="https://img.shields.io/badge/GitHub-caotrongphuoc-181717?style=for-the-badge&logo=github&logoColor=white"/>
</a>

<a href="https://www.linkedin.com/in/cao-trong-phuoc/">
  <img src="https://img.shields.io/badge/LinkedIn-Cao%20Trong%20Phuoc-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
</a>

<a href="mailto:caotrongphuoc@gmail.com">
  <img src="https://img.shields.io/badge/Gmail-caotrongphuoc%40gmail.com-EA4335?style=for-the-badge&logo=gmail&logoColor=white"/>
</a>
