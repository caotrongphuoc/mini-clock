<h1 align="center">Coding rules and style guide</h1>

Tài liệu này định nghĩa các quy ước đặt tên, code style, commit message và quy ước đặt tên tài liệu đang áp dụng trong project, kèm hướng dẫn cài đặt và sử dụng `clang-format`. Mục tiêu là đảm bảo code do các thành viên đóng góp luôn nhất quán về hình thức, dễ tracking qua các công cụ search, review và version control. Đây là những đúc kết sau khi hoàn thành game Zomwar, các bạn thấy phù hợp có thể tham khảo và áp dụng.

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
- [VI. Quy ước đặt tên file tài liệu](#vi-quy-ước-đặt-tên-file-tài-liệu)
- [VII. Tài liệu tham khảo](#vii-tài-liệu-tham-khảo)

---

## I. Quy ước đặt tên

Các quy ước dưới đây được rút ra trực tiếp từ source code hiện có. Code mới phải tuân thủ đúng convention để tooling, search và reviewer làm việc nhất quán.

**Quy ước viết tên (case style) dùng trong tài liệu này:**

| Kiểu | Mô tả | Ví dụ trong project | Áp dụng cho |
|---|---|---|---|
| `lower_snake_case` | Chữ thường, các từ ngăn cách bằng dấu gạch dưới `_` | `wave_warning_active`, `zw_game_score` | Biến, hàm, typedef, tên file source, tên folder |
| `UPPER_SNAKE_CASE` | Chữ hoa, các từ ngăn cách bằng dấu gạch dưới `_` | `BULLET_NUMBER`, `ZW_GAME_BORDER_SETUP`, `AC_TASK_DISPLAY_ID` | Hằng số `#define`, signal enum, task ID, macro |
| `kebab-case` | Chữ thường, các từ ngăn cách bằng dấu gạch ngang `-` | `02-guide-coding-rules.md` | Tên file tài liệu trong `docs/` |

### 1. Folder

Đặt tên theo `lower_snake_case`. Tổ chức theo tính năng (feature-based), không tổ chức theo loại file.

```
application/sources/app/
  game/
    game_<ten_game>/      # folder chứa toàn bộ source code các đối tượng của game,
                          # ví dụ: game_zomwar
  screens/                # folder chứa toàn bộ source code các màn hình hiển thị
  ...
```

### 2. File source và header

File source và header luôn mang prefix module để xác định nhanh module ngay khi nhìn tên file:

| Prefix | Ý nghĩa | Ví dụ |
|---|---|---|
| `scr_*` | Handler của một screen | `scr_game_zomwar.cpp`, `scr_game_over.h` |
| `zw_game_*` | Object thuộc game Zomwar | `zw_game_bang.cpp`, `zw_game_border.h` |

Mỗi game định nghĩa prefix ngắn riêng (ví dụ `zw_game_*` cho Zomwar) và áp dụng đồng nhất cho toàn bộ file trong folder game đó.

Quy ước đuôi file: `.h` cho header, `.cpp` cho implementation (project biên dịch bằng C++).

### 3. Header guard

Sử dụng pattern `__<TÊN_FILE>_H__`, viết hoa toàn bộ, khớp chính xác với tên file:

```cpp
#ifndef __ZW_GAME_BANG_H__
#define __ZW_GAME_BANG_H__
...
#endif //__ZW_GAME_BANG_H__
```

### 4. Macro và hằng số biên dịch

Đặt tên theo `UPPER_SNAKE_CASE`. Luôn bọc giá trị số trong dấu ngoặc đơn để tránh lỗi khi macro được expand.

**Quy tắc bắt buộc: macro thuộc về object nào thì PHẢI mang tên object đó làm prefix.**

Pattern: `<OBJECT>_<PROPERTY>` hoặc `<OBJECT>_<ACTION>` — object luôn đứng trước. Đọc tên macro là biết ngay nó thuộc module nào, grep theo object là ra toàn bộ hằng số của module đó.

| Loại hằng | Đặt đúng |
|---|---|
| Số lượng | `ZOMBIE_NUMBER`, `BULLET_NUMBER`, `BANG_NUMBER` |
| Tọa độ | `GUNNER_AXIS_X`, `CAR_AXIS_X` |
| Bước di chuyển | `GUNNER_STEP_AXIS_Y`, `BULLET_STEP_AXIS_X` |
| Kích thước bitmap | `GUNNER_SIZE_BITMAP_X`, `BANG_SIZE_BITMAP_I_X` |
| Thời gian / chu kỳ | `BORDER_WAVE_SCORE_INTERVAL`, `TOMBSTONE_SPAWN_INTERVAL` |
| Hành vi / giới hạn | `ZOMBIE_RISE_TICKS`, `ZOMBIE_SPEED_MAX`, `CAR_HIT_RANGE_Y` |

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

**Ngoại lệ — macro mức system / project:** macro thuộc về toàn bộ game (không gắn với một object cụ thể) dùng project prefix `ZW_GAME_*`:

```cpp
#define ZW_GAME_TIME_TICK_INTERVAL (100)
#define ZW_GAME_TIME_EXIT_INTERVAL (3000)
```

Gom hằng số liên quan vào đúng header module (`zw_game_bullet.h` chứa hằng bullet, `zw_game_gunner.h` chứa hằng gunner...). Tuyệt đối không để magic number rải rác trong `.cpp`.

### 5. Signal (giá trị enum)

Signal là **hợp đồng công khai** giữa các task. Luôn dùng full prefix — không viết tắt, kể cả trong comment, tài liệu, hay sequence diagram.

| Pattern | Áp dụng cho | Ví dụ |
|---|---|---|
| `<GAME>_<OBJECT>_<ACTION>` | Signal theo từng game | `ZW_GAME_BORDER_CHECK_GAME_OVER` |

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

Đặt tên theo pattern `<PREFIX>_<TÊN>_ID`, viết hoa toàn bộ, đăng ký trong `task_list.h`:

```cpp
AC_TASK_DISPLAY_ID
ZW_GAME_GUNNER_ID
ZW_GAME_BORDER_ID
```

Handler tương ứng trong `task_list.cpp` giữ nguyên tên, thay hậu tố `_ID` bằng `_handle` và viết thường:

```cpp
{ZW_GAME_BORDER_ID, TASK_PRI_LEVEL_4, zw_game_border_handle},
```

### 7. Kiểu dữ liệu và typedef

Đặt tên theo `lower_snake_case` kèm hậu tố `_t`. Struct để vô danh; typedef là tên public:

```cpp
typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t action_image;
    bool    visible;
} zw_game_gunner_t;
```

Kiểu do framework cung cấp cũng tuân thủ pattern này (`ak_msg_t`, `view_screen_t`).

### 8. Hàm

Đặt tên theo `lower_snake_case` kèm prefix tên module, để `grep` prefix là ra toàn bộ entry point của module đó:

```cpp
void   zw_game_border_handle(ak_msg_t* msg);
void   zw_game_bang_spawn(int16_t x, uint8_t y);
void   zw_game_zombie_spawn_from_tombstone(uint8_t i, int16_t x, uint8_t y);
int8_t zw_game_car_find_nearest(uint8_t zy);
```

### 9. Biến

Đặt tên theo `lower_snake_case`. Không bắt đầu bằng dấu gạch dưới.

- **Biến global chia sẻ giữa các module:** khai báo `extern` trong header, định nghĩa đúng một lần trong `.cpp` của module sở hữu.

  ```cpp
  // zw_game_border.h
  extern uint16_t zw_game_score;
  extern uint8_t  wave_level;
  extern bool     wave_warning_active;
  ```

- **Biến nội bộ của module:** khai báo `static` trong `.cpp`.

  ```cpp
  // scr_game_zomwar.cpp
  static uint8_t zw_game_state;
  static uint8_t gunner_dir = GUNNER_DIR_NONE;
  ```

- **Biến local:** ngắn gọn, mô tả đúng vai trò. Biến đếm vòng lặp có thể dùng `i`, `j`, `k` khi scope rõ ràng.

State của object thuộc task nào thì gắn tên object đó (`gunner.y`, `bang[i].visible`, `wave_warning_timer`); không nhét state cross-cutting vào `.cpp` của module khác.

---

## II. Code style (clang-format)

Repo đã có sẵn file `.clang-format` ở thư mục gốc, có thể tham khảo:

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

Ý nghĩa các setting khác mặc định:

| Setting | Tác dụng |
|---|---|
| `UseTab: ForIndentation`, `IndentWidth: 4`, `TabWidth: 4` | Tab chỉ dùng để indent, không dùng để align. Một tab tương đương 4 cột. |
| `ColumnLimit: 0` | Không tự động xuống dòng. Tác giả tự quyết định vị trí break line. |
| `BreakBeforeBraces: Allman` | Dấu `{` luôn ở dòng riêng — áp dụng cho `if`, `for`, function body. |
| `AllowShort*OnASingleLine: false` | Mỗi statement một dòng, kể cả `if` ngắn hay function rỗng. |
| `PointerAlignment: Left` | Viết `int* p`, không phải `int *p`. |
| `SpaceBeforeParens: ControlStatements` | `if (x)`, `for (...)` có space; `func(x)` không có. |
| `IndentCaseLabels: false` | `case` cùng cấp indent với `switch`. |
| `SortIncludes: false` | Thứ tự include có ý nghĩa (BSP → framework → project), không được sort tự động. |

---

## III. Cài đặt clang-format

Hướng dẫn cho Linux (Ubuntu / Debian).

Cài đặt:

```bash
sudo apt update
sudo apt install clang-format
```

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_setup.png" alt="Cài đặt clang-format" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 1:</em></strong> Cài đặt clang-format</p>

Kiểm tra cài đặt:

```bash
clang-format --version
```

Kết quả mong đợi (số version có thể khác):

```
Ubuntu clang-format version 14.x.x
```

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_check_version.png" alt="Kiểm tra phiên bản clang-format" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 2:</em></strong> Kiểm tra phiên bản clang-format</p>

---

## IV. Chạy clang-format

### 1. Chạy bằng command line

Format một file in-place:

```bash
clang-format -i application/sources/app/game/game_zomwar/zw_game_bullet.cpp
```

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_before_format_1.png" alt="Trước khi format" width="500"/></td>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_after_format_1.png" alt="Sau khi format" width="500"/></td>
  </tr>
  <tr>
    <td align="center"><strong><em>Trước khi format</em></strong></td>
    <td align="center"><strong><em>Sau khi format</em></strong></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 3:</em></strong> So sánh trước và sau khi format một file</p>

Format toàn bộ source và header trong `application/sources/app`:

```bash
find application/sources/app -type f \( -name "*.cpp" -o -name "*.h" \) \
    -not -path "*/libraries/*" \
    -exec clang-format -i {} +
```

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_before_format_2_1.png" alt="Trước khi format - file 1" width="500"/></td>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_before_format_2_2.png" alt="Trước khi format - file 2" width="500"/></td>
  </tr>
  <tr>
    <td align="center"><strong><em>Trước khi format (1)</em></strong></td>
    <td align="center"><strong><em>Trước khi format (2)</em></strong></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 4:</em></strong> Trạng thái source code trước khi format hàng loạt</p>

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_after_format_2_1.png" alt="Sau khi format - file 1" width="500"/></td>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_after_format_2_2.png" alt="Sau khi format - file 2" width="500"/></td>
  </tr>
  <tr>
    <td align="center"><strong><em>Sau khi format (1)</em></strong></td>
    <td align="center"><strong><em>Sau khi format (2)</em></strong></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 5:</em></strong> Trạng thái source code sau khi format hàng loạt</p>

### 2. Tích hợp với VSCode

**Bước 1.** Cài extension **C/C++** (Microsoft) — extension này đi kèm `clang-format` và tự động đọc file `.clang-format` trong repo.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_download_extension.png" alt="Cài extension C/C++" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 6:</em></strong> Cài đặt extension C/C++</p>

**Bước 2.** Mở workspace settings (`.vscode/settings.json`) và thêm cấu hình:

```json
{
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "ms-vscode.cpptools",
    "C_Cpp.clang_format_style": "file",
    "C_Cpp.clang_format_fallbackStyle": "LLVM"
}
```

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_open_workspace_setting_json.png" alt="Mở workspace settings" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 7:</em></strong> Mở file workspace settings</p>

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_config.png" alt="Cấu hình JSON" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 8:</em></strong> Nội dung cấu hình JSON</p>

**Bước 3.** Format file hiện tại bằng phím tắt <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>I</kbd> (Windows / Linux).

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_before_format_3.png" alt="Trước khi format" width="500"/></td>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_after_format_3.png" alt="Sau khi format" width="500"/></td>
  </tr>
  <tr>
    <td align="center"><strong><em>Trước khi format</em></strong></td>
    <td align="center"><strong><em>Sau khi format bằng <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>I</kbd></em></strong></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 9:</em></strong> So sánh trước và sau khi format bằng phím tắt</p>

**Bước 4.** Bật `formatOnSave` để editor tự format mỗi khi save, đảm bảo không commit code lệch format.

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_open_setting.png" alt="Mở settings" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 10:</em></strong> Mở settings của VSCode</p>

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_on_save.png" alt="Bật Format On Save" width="1000"/></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 11:</em></strong> Kích hoạt tùy chọn Format On Save</p>

<table align="center">
  <tr>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_before_format_4.png" alt="Trước khi format" width="500"/></td>
    <td align="center"><img src="../resources/images/guide-coding-rules/clang_format_after_format_4.png" alt="Sau khi format" width="500"/></td>
  </tr>
  <tr>
    <td align="center"><strong><em>Trước khi format</em></strong></td>
    <td align="center"><strong><em>Sau khi format (tự động khi save)</em></strong></td>
  </tr>
</table>
<p align="center"><strong><em>Hình 12:</em></strong> So sánh trước và sau khi format tự động lúc save file</p>

---

## V. Quy ước commit message

Mọi commit phải tuân thủ format `[ACTION] mô tả ngắn` để lịch sử git dễ đọc và dễ filter.

### 1. Workflow

```bash
git add .                                     # stage toàn bộ thay đổi
git commit -m "[ACTION] mô tả ngắn"           # tag bắt buộc, mô tả ngắn gọn
git push                                      # đẩy lên remote
```

Khi chỉ cần stage một số file cụ thể, thay `git add .` bằng `git add <path>` để tránh commit nhầm file rác.

### 2. Action tag

| Tag | Khi nào dùng |
|---|---|
| `[ADD]` | Thêm file, feature, asset, hoặc tài liệu mới |
| `[UPDATE]` | Cập nhật code có sẵn — refactor, đổi tên, tinh chỉnh logic, nâng version |
| `[FIX]` | Sửa bug đã có hoặc lỗi build / lỗi format |
| `[REMOVE]` | Xoá file, feature, hoặc code chết |
| `[DOC]` | Chỉ chạm vào tài liệu (`docs/`, `README.md`, comment block lớn) |
| `[MERGE]` | Merge branch (thường do tool tự sinh, không sửa tay) |

### 3. Style mô tả

- Tag viết hoa toàn bộ trong `[]`, theo sau đúng một dấu cách rồi mới đến mô tả.
- Mô tả viết thường, dạng mệnh lệnh (`add`, `fix`, `rename`, `move`...), không có dấu chấm cuối câu.
- Giữ độ dài trong khoảng 70 ký tự — nếu dài hơn thì rút gọn hoặc đưa chi tiết xuống body commit.
- Khi thao tác trên module / signal / file cụ thể, gọi tên trực tiếp để dễ grep lịch sử.

### 4. Ví dụ tốt

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

### 5. Ví dụ cần tránh

```text
update                          # thiếu tag
[update] fix something          # tag phải viết hoa
[ADD] Added new file.           # không dùng thì quá khứ và chấm câu
[FIX] fix bug                   # quá mơ hồ, không biết bug gì
[ADD] zw_game_border.cpp + zw_game_zombie.cpp + scr_game_zomwar.cpp ... # quá dài, nên gộp theo chủ đề
```

---

## VI. Quy ước đặt tên file tài liệu

Tài liệu trong `docs/` tuân thủ format `<NN>-<category>-<topic>.md`:

| Thành phần | Quy ước | Ví dụ |
|---|---|---|
| `NN` | Số thứ tự 2 chữ số, đánh từ `01`. Phản ánh thứ tự đọc — guide đứng trước, design đứng sau. | `01`, `02`, `03` |
| `category` | Phân loại tài liệu. Chỉ dùng các giá trị đã định nghĩa, không tự thêm category mới. | `guide`, `design` |
| `topic` | Chủ đề chính, viết theo `kebab-case` (chữ thường, các từ ngăn cách bằng `-`). | `getting-started`, `coding-rules`, `sequence-object` |

Các category đang sử dụng:

| Category | Mục đích | Nội dung điển hình |
|---|---|---|
| `guide` | Hướng dẫn quy trình, setup, workflow cho contributor | Getting started, coding rules, build & flash, debugging |
| `design` | Mô tả kiến trúc và hành vi runtime của hệ thống | Sequence diagram, dataflow, task ownership, signal map |

Ví dụ các file đang có trong repo:

```
docs/
├── 01-guide-getting-started.md
├── 02-guide-coding-rules.md
├── 03-design-sequence-object.md
└── 04-design-sequence-runtime.md
```

Một số lưu ý quan trọng:

- File tài liệu (`.md`) dùng `kebab-case` (dấu gạch ngang). File source và folder dùng `snake_case` (dấu gạch dưới). Đây là sự khác biệt có chủ đích: `kebab-case` là convention chuẩn cho slug Markdown và URL.
- Ảnh đi kèm tài liệu lưu trong `resources/images/<topic_dir>/`, trong đó `<topic_dir>` viết theo quy ước folder (snake_case). Ví dụ: `resources/images/getting_started/`.
- Khi thêm tài liệu mới, tiếp tục số thứ tự nối tiếp số lớn nhất hiện có và gom theo nhóm category để giữ thứ tự đọc tự nhiên.
- Đổi tên file tài liệu phải dùng `git mv` để lịch sử rename được track đúng.

---

## VII. Tài liệu tham khảo

- [Naming convention — Multi-word identifiers (Wikipedia)](https://en.wikipedia.org/wiki/Naming_convention_(programming)#Multiple-word_identifiers) — bảng định nghĩa `snake_case`, `SCREAMING_SNAKE_CASE`, `kebab-case` dùng ở Mục I và VI.
- [Clang-Format — Configurable Format Style Options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#configurable-format-style-options) — mô tả từng key trong file `.clang-format` ở Mục II.
- [LLVM Coding Standards — Source Code Formatting](https://llvm.org/docs/CodingStandards.html#source-code-formatting) — base style kế thừa qua `BasedOnStyle: LLVM` ở Mục II.
- [Pro Git — Recording Changes to the Repository](https://git-scm.com/book/en/v2/Git-Basics-Recording-Changes-to-the-Repository) — workflow `git add` / `git commit` / `git push` ở Mục V.

---

## Contact & Support

<p style="font-size: 20px;"><strong>Cao Trong Phuoc</strong> - Software Engineer - Embedded Systems</p>

```
Cảm ơn bạn đã ghé thăm repository này.
Nếu có câu hỏi, góp ý hoặc phản hồi về project hay về lĩnh vực firmware development,
đừng ngần ngại liên hệ trực tiếp với mình.
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
