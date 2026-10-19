# Mini Clock 2:1 banners

- [Animated GIF](mini_clock_banner.gif): the README banner.
- [Static PNG cover](mini_clock_banner_2x1.png): 1280×640 (2:1), suitable for sharing or uploading as
  a repository social preview. Clicking the README banner opens it; adding this file does not
  change GitHub repository settings.

The banner is generated documentation art, not a firmware screen or a camera recording. It follows
the layout of the [Zomwar](https://github.com/caotrongphuoc/zomwar-game) and Eleven Meter banners — a
2:1 canvas, a large pixel title, a row of icons, and five preview panels — with a local blue clock
theme. No artwork from those projects is copied.

- The title and captions use the firmware's own 5×7 OLED font (`glcdfont.cpp`).
- The icon row is the seven menu icons from `screens_bitmap.cpp`, unchanged and enlarged ×3; one is
  highlighted at a time.
- The panels are the captured OLED previews from [`screens/`](screens/) (menu, digital clock, world
  clock, alarm ringing) and the repository's AK Base Kit photograph. The ringing panel shows the
  08:00 test alarm used during capture.
- The faint clock face behind the title and its moving second hand are drawn by the script.

The GIF has 18 frames with 500 ms holds (9 s loop) and a fixed 128-colour palette. OLED previews use
nearest-neighbour resizing; the kit photograph uses Lanczos.
