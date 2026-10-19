#!/usr/bin/env python3
"""Render the README banner: a 1280x640 GIF and its static 2:1 PNG cover.

Built from repository assets only: the OLED font (glcdfont.cpp), the menu icon
bitmaps (screens_bitmap.cpp), the captured screen previews in
resources/images/screens/ (tools/render_screens.py) and the kit photograph.
The layout follows the Zomwar / Eleven Meter banners (2:1 canvas, pixel title,
an icon row, five preview panels) with a local blue clock theme.

Usage (from the repository root):
  python3 tools/render_banner.py
"""

import math
import re
from pathlib import Path

from PIL import Image, ImageDraw, ImageOps, ImageSequence

ROOT = Path(__file__).resolve().parents[1]
FONT_SOURCE = ROOT / "application/sources/driver/Adafruit_oled_drv/glcdfont.cpp"
BITMAP_SOURCE = ROOT / "application/sources/app/screens/screens_bitmap.cpp"
SCREENS = ROOT / "resources/images/screens"
OUTPUT = ROOT / "resources/images/mini_clock_banner.gif"
STATIC_OUTPUT = ROOT / "resources/images/mini_clock_banner_2x1.png"

WIDTH, HEIGHT = 1280, 640
WHITE = (236, 243, 252)
BLUE = (96, 172, 255)
DIM = (120, 150, 190)
FRAME = (40, 78, 130)
STEP_MS = 500
LOOP_MS = 9000

ICONS = [("bitmap_clock_menu_clock", "CLOCK"), ("bitmap_clock_menu_alarm", "ALARM"),
         ("bitmap_clock_menu_stopwatch", "STOPWATCH"), ("bitmap_clock_menu_timer", "TIMER"),
         ("bitmap_world_clock", "WORLD"), ("bitmap_calendar_display", "CALENDAR"),
         ("bitmap_clock_menu_setting", "SETTING")]


def parse_array(path, name):
	text = path.read_text()
	body = re.search(re.escape(name) + r"\[\][^{]*\{(.*?)\}", text, re.S).group(1)
	return [int(v, 16) for v in re.findall(r"0x[0-9A-Fa-f]{1,2}", body)]


def lettering(font, text, scale):
	mask = Image.new("1", (len(text) * 6 - 1, 7))
	for index, character in enumerate(text):
		for column in range(5):
			for row in range(7):
				if font[ord(character) * 5 + column] & (1 << row):
					mask.putpixel((index * 6 + column, row), 1)
	return mask.resize((mask.width * scale, 7 * scale), Image.NEAREST)


def label(canvas, font, text, y, scale=2, color=BLUE, center=WIDTH // 2):
	mask = lettering(font, text, scale)
	canvas.paste(color, (center - mask.width // 2, y), mask)


def icon(data, size=22, scale=3):
	"""Adafruit drawBitmap layout: row-major, MSB first, rows padded to bytes."""
	stride = (size + 7) // 8
	mask = Image.new("1", (size, size))
	for y in range(size):
		for x in range(size):
			if data[y * stride + x // 8] & (0x80 >> (x % 8)):
				mask.putpixel((x, y), 1)
	return mask.resize((size * scale, size * scale), Image.NEAREST)


def load_animation(path):
	with Image.open(path) as image:
		return [(frame.convert("RGB"), frame.info.get("duration", 1000)) for frame in ImageSequence.Iterator(image)]


def at_time(frames, time_ms):
	time_ms %= sum(duration for _, duration in frames)
	for image, duration in frames:
		if time_ms < duration:
			return image
		time_ms -= duration
	return frames[-1][0]


def background():
	canvas = Image.new("RGB", (WIDTH, HEIGHT))
	px = canvas.load()
	for y in range(HEIGHT):
		for x in range(WIDTH):
			glow = max(0.0, 1 - ((x - 640) / 820) ** 2 - ((y - 250) / 430) ** 2)
			shade = 0.85 if y % 3 == 0 else 1
			px[x, y] = tuple(int((a + b * glow) * shade) for a, b in ((4, 6), (7, 16), (14, 34)))
	return canvas


def clock_face(draw, second):
	"""Faint wall-clock behind the title; the second hand moves with the GIF."""
	cx, cy, r = 640, 214, 206
	draw.ellipse((cx - r, cy - r, cx + r, cy + r), outline=(22, 44, 76), width=2)
	for tick in range(60):
		a = math.radians(tick * 6)
		inner = r - (16 if tick % 5 == 0 else 7)
		draw.line((cx + inner * math.sin(a), cy - inner * math.cos(a), cx + (r - 3) * math.sin(a),
		           cy - (r - 3) * math.cos(a)), fill=(26, 52, 88), width=3 if tick % 5 == 0 else 1)
	a = math.radians(second * 6)
	draw.line((cx, cy, cx + (r - 26) * math.sin(a), cy - (r - 26) * math.cos(a)), fill=(30, 62, 104), width=2)


def main():
	font = parse_array(FONT_SOURCE, "font")
	icons = [(icon(parse_array(BITMAP_SOURCE, name)), caption) for name, caption in ICONS]
	panels = [("MENU", load_animation(SCREENS / "scr_clock_menu.gif")),
	          ("DIGITAL", load_animation(SCREENS / "scr_clock_main.gif")),
	          ("AK BASE KIT", None),
	          ("WORLD CLOCK", load_animation(SCREENS / "scr_clock_world.gif")),
	          ("ALARM", load_animation(SCREENS / "scr_clock_ringing.gif"))]
	with Image.open(ROOT / "resources/images/board/ak-embedded-base-kit-version-3.jpg") as source:
		board = ImageOps.contain(source.convert("RGB"), (224, 112), Image.LANCZOS)

	base = background()
	title = lettering(font, "MINI CLOCK", 15)
	x = (WIDTH - title.width) // 2
	frames = []
	for step in range(LOOP_MS // STEP_MS):
		time_ms = step * STEP_MS
		frame = base.copy()
		draw = ImageDraw.Draw(frame)
		clock_face(draw, (time_ms // 1000) % 60 * 6)

		frame.paste((22, 52, 104), (x + 9, 73), title)
		frame.paste((70, 110, 170), (x - 4, 60), title)
		frame.paste(WHITE, (x, 64), title)
		label(frame, font, "ALARMS. TIMERS. 32 CITIES.", 218, 4)
		label(frame, font, "STM32L151 / ACTIVE KERNEL / AK BASE KIT", 266, 2, DIM)

		# Icon row: the menu icons, one highlighted at a time
		draw.rectangle((28, 300, 1252, 416), outline=FRAME, width=2)
		lit = step % len(icons)
		for index, (mask, caption) in enumerate(icons):
			center = 112 + index * 176
			color = WHITE if index == lit else (70, 98, 140)
			frame.paste(color, (center - mask.width // 2, 314), mask)
			label(frame, font, caption, 392, 2, BLUE if index == lit else (60, 86, 124), center)

		for index, (caption, animation) in enumerate(panels):
			left = 16 + index * 252
			label(frame, font, caption, 438, 2, center=left + 120)
			draw.rectangle((left, 460, left + 239, 587), fill=(2, 4, 8), outline=BLUE, width=2)
			image = board if animation is None else at_time(animation, time_ms).resize((224, 112), Image.NEAREST)
			frame.paste(image, (left + (240 - image.width) // 2, 468))

		label(frame, font, "DIGITAL DESK CLOCK ON THE AK EMBEDDED BASE KIT", 608, 2, DIM)
		frames.append(frame)

	# A fixed palette keeps the white OLED pixels stable between frames.
	palette = frames[0].quantize(colors=128, dither=Image.NONE)
	encoded = [frame.quantize(palette=palette, dither=Image.NONE) for frame in frames]
	encoded[0].save(OUTPUT, save_all=True, append_images=encoded[1:], duration=STEP_MS,
	                loop=0, disposal=2, optimize=False)
	frames[0].save(STATIC_OUTPUT, optimize=True)
	print(f"{OUTPUT.relative_to(ROOT)}: {len(frames)} frames x {STEP_MS} ms")
	print(f"{STATIC_OUTPUT.relative_to(ROOT)}: {WIDTH}x{HEIGHT} static 2:1 cover")


if __name__ == "__main__":
	main()
