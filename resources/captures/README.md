# OLED framebuffer captures

[`mc_clock_screens_uart.json`](mc_clock_screens_uart.json) is the source of every screen image in
[`../images/screens/`](../images/screens/). It records the real kit's OLED through the Type-C CH340
UART at 115200 8N1 on **2026-10-19**.

## Firmware

The capture image is the release source plus one change: `LOGIN_PRINT_EN` and `SYS_PRINT_EN` were
enabled in `application/Makefile` so the shell can print. Its updater checksum is **`0x9211`**
(80,496-byte image). The other four print/debug flags stayed off, and the gates were restored
afterwards; the release image prints nothing on the console.

## How the frames were taken

[`tools/capture_screens.py`](../../tools/capture_screens.py) drives the clock with the `btn` shell
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
- The alarm editor frame and the two ringing blink phases were re-taken with the same firmware
  after the first run caught them mid-update; both ringing phases are kept because the title
  bar blinks once per second.

[`tools/render_screens.py`](../../tools/render_screens.py) only enlarges pixels (nearest neighbour,
×4 → 512×256); the saved framebuffer bytes are not edited.

```bash
python3 tools/capture_screens.py --port /dev/ttyUSB0      # diagnostic build, clock on Digital
python3 tools/render_screens.py
```
