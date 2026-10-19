#!/usr/bin/env python3
"""Render the captured OLED framebuffers into README screen images.

Reads resources/captures/mc_clock_screens_uart.json (written by
tools/capture_screens.py) and writes resources/images/screens/*.png|gif.
Pixels are only enlarged (nearest neighbour, x4 -> 512x256); the captured
framebuffer bytes are not edited.

Usage (from the repository root):
  python3 tools/render_screens.py
"""

import json
import os

from PIL import Image

SRC = "resources/captures/mc_clock_screens_uart.json"
OUT = "resources/images/screens"
SCALE = 4

# output name -> list of (screen, label) groups, in playback order.
# A single frame becomes a PNG; several frames also become a looping GIF.
OUTPUTS = {
	"scr_clock_main": [("scr_clock_main", "digital")],
	"scr_clock_menu": [("scr_clock_menu", None)],
	"scr_idle": [("scr_idle", "balls")],
	"scr_clock_analog": [("scr_clock_analog", "analog")],
	"scr_clock_alarm_list": [("scr_clock_alarm", "list")],
	"scr_clock_alarm_edit": [("scr_clock_alarm", "edit")],
	"scr_clock_stopwatch_help": [("scr_clock_stopwatch", "help")],
	"scr_clock_stopwatch": [("scr_clock_stopwatch", "running")],
	"scr_clock_timer_set": [("scr_clock_timer", "set")],
	"scr_clock_timer": [("scr_clock_timer", "running")],
	"scr_clock_world": [("scr_clock_world", None)],
	"scr_clock_calendar_month": [("scr_clock_calendar", "month_event")],
	"scr_clock_calendar_day": [("scr_clock_calendar", "day")],
	"scr_clock_calendar_edit": [("scr_clock_calendar", "edit")],
	"scr_clock_setting": [("scr_clock_setting", "list")],
	"scr_clock_time_setting": [("scr_clock_time_setting", "time")],
	"scr_clock_setting_display": [("scr_clock_setting_display", "display")],
	"scr_clock_setting_sound": [("scr_clock_setting_sound", "sound")],
	"scr_clock_ringing": [("scr_clock_ringing", "alarm")],
}


def frame_image(data_hex, width, height):
	"""Page-major, LSB = top pixel (Adafruit OLED driver layout)."""
	data = bytes.fromhex(data_hex)
	img = Image.new("1", (width, height), 0)
	px = img.load()
	for x in range(width):
		for y in range(height):
			if (data[(y // 8) * width + x] >> (y % 8)) & 1:
				px[x, y] = 1
	return img.resize((width * SCALE, height * SCALE), Image.NEAREST).convert("RGB")


def main():
	record = json.load(open(SRC))
	width, height = record["width"], record["height"]
	os.makedirs(OUT, exist_ok=True)

	for name, groups in OUTPUTS.items():
		frames = [f for screen, label in groups for f in record["frames"]
		          if f["screen"] == screen and (label is None or f["label"] == label)]
		if not frames:
			raise SystemExit(f"{name}: no frames in {SRC}")

		images = [frame_image(f["data_hex"], width, height) for f in frames]
		images[0].save(os.path.join(OUT, name + ".png"))
		outputs = [name + ".png"]

		if len(images) > 1:
			images[0].save(os.path.join(OUT, name + ".gif"), save_all=True, append_images=images[1:],
			               duration=[f["duration_ms"] for f in frames], loop=0, optimize=False)
			outputs.append(name + ".gif")

		print(f"{name:28s} {len(frames)} frame(s) -> {', '.join(outputs)}")


if __name__ == "__main__":
	main()
