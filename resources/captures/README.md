# OLED framebuffer captures

## Screen previews

[`mc_clock_screens_uart.json`](mc_clock_screens_uart.json) is the source of every screen image in
[`../images/screens/`](../images/screens/). It records the real kit's OLED through the Type-C CH340
UART at 115200 8N1 on **2026-10-19**.

## Firmware

The capture image is the release source plus one change: `LOGIN_PRINT_EN` and `SYS_PRINT_EN` were
enabled in `application/Makefile` so the shell can print. Its updater checksum is **`0x9211`**
(80,496-byte image). The other four print/debug flags stayed off, and the gates were restored
afterwards; the release image prints nothing on the console.

## How the frames were taken

A capture script (development tooling, removed after the documentation was finished) drove the clock with the `btn` shell
command (it posts the same Display signals as the physical buttons) and reads each frame with
`lcd d`. It adds three sample alarms (06:00 on, 07:00 off, 09:00 on) and one calendar event, sets
the RTC to 05:59:57 to trigger the ringing page, and afterwards deletes the samples and re-syncs the
RTC to Vietnam time. This is automated screen-input capture, not physical button testing or camera
footage.

- Frames are 128×64, page-major, LSB = top pixel (`"format": "ak-oled-page-major-lsb"`).
- `section` / `screen` / `label` name the step and the screen; `captured_at` is Vietnam time.
- `duration_ms` is a presentation hold chosen for readability. One `lcd d` takes about 0.6 s over
  UART, so animated previews are sampled about once per second; menu and World Clock slides are
  shown as their resting frames, not as the 50 ms slide animation.
- The ringing frames were re-taken separately with the same `0x9211` image (an alarm at 08:00 and
  the RTC set to 07:59:57); four consecutive frames keep both blink phases of the title bar.

Rendering only enlarged pixels (nearest neighbour, ×4 → 512×256); the saved framebuffer bytes
are not edited.


## Demo

[`mc_clock_demo_uart.json`](mc_clock_demo_uart.json) is the source of the README's
[demo GIF](../videos/mini_clock_demo.gif). It was recorded with the same `0x9211` diagnostic image on
**2026-10-19** by the same tooling: Digital → Menu → Analog →
World Clock (Vietnam, England, back to Vietnam) → Stopwatch with one lap → add a 08:00 alarm → the RTC
is set to 07:59:55 → the alarm rings → any key dismisses it → Digital.

Each frame keeps its real capture time (`t`, Unix seconds). The demo GIF holds every frame for the real gap to the next one (about 0.6–1.5 s, capped at 1.5 s) and the last
frame for 2 s, so the GIF plays close to real time but samples only about one frame per second; the
50 ms menu slide is not visible. Pixels are only enlarged ×4. The sample alarm was deleted and the RTC
re-synced afterwards.
