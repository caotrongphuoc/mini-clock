#!/usr/bin/env python3
"""Render the README's basic clock sequence (Figure 8) with Pillow.

Each row is a high-level summary of the current message flow; exact signal
names and payloads are in docs/02-design-sequence-object.md and
docs/03-design-sequence-runtime.md.

Usage (from the repository root):
  python3 tools/render_clock_sequence.py
"""

import argparse
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "resources/images/design-sequence/mc_clock_sequence.png"
WIDTH = 1920
ROW_HEIGHT = 66
BACKGROUND = "#0b1220"
TEXT = "#e5e7eb"
MUTED = "#94a3b8"
PARTICIPANTS = (
	("User", "#84cc16"),
	("Display", "#60a5fa"),
	("RTC", "#f472b6"),
	("Time", "#c084fc"),
	("World", "#22d3ee"),
	("Alarm", "#facc15"),
	("Timer", "#fb923c"),
	("Calendar", "#34d399"),
	("EEPROM", "#a3a3a3"),
)

PHASES = (
	("01  BOOT", (
		("Display", "EEPROM", "Read settings, alarms and events"),
		("Display", "Display", "Apply mute, brightness and city; startup melody"),
		("Alarm", "RTC", "SET_ALARM_REQ: next enabled alarm"),
		(None, None, "app_start_timer() starts the RTC task's 1 s service tick. After the 2 s logo the Digital screen opens."),
	)),
	("02  SHOWING THE TIME", (
		("Display", "Time", "TIME_UPDATE (screen tick, 1 s)"),
		(None, None, "Time reads the PCF85063A through the RTC driver and converts it to the selected city."),
		("Time", "Display", "RENDER: redraw with the new second"),
		("User", "Display", "Mode: menu   Up / Down: item"),
	)),
	("03  EDITING AN ALARM", (
		("User", "Display", "Alarm screen keys"),
		("Display", "Alarm", "SELECT / UP / DOWN / NEXT_FIELD"),
		("Alarm", "Display", "RENDER: show the new list or field"),
		("Alarm", "EEPROM", "Save the alarm bank (only when it changed)"),
		("Alarm", "RTC", "SET_ALARM_REQ: next enabled alarm"),
		("RTC", "RTC", "Write alarm registers; retry each second if not acknowledged"),
	)),
	("04  EVERY SECOND, ON EVERY SCREEN", (
		("RTC", "RTC", "Poll the alarm flag (AF)"),
		("RTC", "Timer", "TIMER_UPDATE"),
		("RTC", "Calendar", "CALENDAR_UPDATE: check reminders"),
		("Timer", "Display", "RENDER while counting down"),
	)),
	("05  RINGING", (
		("RTC", "Alarm", "ALARM_FIRED (flag was set)"),
		("Alarm", "Display", "SHOW_RINGING {ALARM, melody}"),
		(None, None, "A timer at zero and a due reminder send the same request as TIMER / EVENT. A newer request replaces the current one."),
		("Display", "Display", "Ringing page; melody every 5 s; stop after 60 s"),
		("User", "Display", "Any key"),
		("Display", "Alarm", "ALARM_DISMISS"),
		("Alarm", "RTC", "SET_ALARM_REQ: re-arm the next alarm"),
	)),
	("06  CHANGING THE TIME OR THE CITY", (
		("User", "Display", "Setting > Time: OK"),
		("Display", "RTC", "SET_TIME_REQ"),
		("RTC", "Alarm", "ALARM_REARM after the time is written"),
		("User", "Display", "World Clock: Up / Down"),
		("Display", "World", "SELECT_UP / SELECT_DOWN"),
		("World", "EEPROM", "Save the selected city"),
		("World", "Alarm", "ALARM_REARM: alarms follow the new time zone"),
	)),
)


def render(font_dir: Path) -> None:
	font = ImageFont.truetype(str(font_dir / "DejaVuSans.ttf"), 22)
	small = ImageFont.truetype(str(font_dir / "DejaVuSans.ttf"), 21)
	heading = ImageFont.truetype(str(font_dir / "DejaVuSans-Bold.ttf"), 25)
	title = ImageFont.truetype(str(font_dir / "DejaVuSans-Bold.ttf"), 37)
	height = 215 + sum(76 + len(rows) * ROW_HEIGHT for _, rows in PHASES) + 85
	image = Image.new("RGB", (WIDTH, height), BACKGROUND)
	draw = ImageDraw.Draw(image)
	step = (WIDTH - 220) // (len(PARTICIPANTS) - 1)
	centers = {name: 110 + index * step for index, (name, _) in enumerate(PARTICIPANTS)}
	draw.text((40, 24), "MINI CLOCK  /  BASIC CLOCK SEQUENCE", font=title, fill=TEXT)
	draw.text((40, 80), "High-level message flow. Time runs downward; arrows are AK messages, and a loop is work inside one task.", font=small, fill=MUTED)

	for name, color in PARTICIPANTS:
		x = centers[name]
		draw.rounded_rectangle((x - 92, 132, x + 92, 188), radius=7, fill=color)
		draw.text((x, 160), name, anchor="mm", font=heading, fill=BACKGROUND)
		for y in range(200, height - 55, 15):
			draw.line((x, y, x, min(y + 7, height - 55)), fill=color, width=2)

	y = 215
	for label, rows in PHASES:
		draw.rectangle((20, y, WIDTH - 20, y + 50), fill="#1e3050")
		draw.text((40, y + 25), label, anchor="lm", font=heading, fill=TEXT)
		y += 76
		for sender, receiver, message in rows:
			if sender is None:
				draw.rounded_rectangle((32, y - 12, WIDTH - 32, y + 40), radius=6, fill="#16233a")
				draw.text((WIDTH // 2, y + 14), message, anchor="mm", font=small, fill="#cbd5e1")
			else:
				start, end = centers[sender], centers[receiver]
				line_y = y + 33
				if start == end:
					bend = start + 55
					draw.line(((start, line_y - 20), (bend, line_y - 20), (bend, line_y), (end, line_y)), fill=TEXT, width=2)
					draw.polygon(((end, line_y), (end + 10, line_y - 5), (end + 10, line_y + 5)), fill=TEXT)
					text_x, anchor = start + 65, "ls"
				else:
					direction = 1 if end > start else -1
					draw.line((start, line_y, end, line_y), fill=TEXT, width=2)
					draw.polygon(((end, line_y), (end - direction * 12, line_y - 6), (end - direction * 12, line_y + 6)), fill=TEXT)
					text_x, anchor = (start + end) // 2, "ms"
				box = draw.textbbox((text_x, y + 19), message, font=font, anchor=anchor)
				if box[0] < 0 or box[2] > WIDTH:
					raise SystemExit(f"label outside image: {message}")
				draw.rectangle((box[0] - 5, box[1] - 3, box[2] + 5, box[3] + 2), fill=BACKGROUND)
				draw.text((text_x, y + 19), message, anchor=anchor, font=font, fill=TEXT)
			y += ROW_HEIGHT

	draw.text((40, height - 35), "Details: docs/02-design-sequence-object.md and docs/03-design-sequence-runtime.md", font=small, fill=MUTED)
	OUTPUT.parent.mkdir(parents=True, exist_ok=True)
	image.save(OUTPUT, optimize=True)
	print(f"Rendered {OUTPUT.relative_to(ROOT)} ({WIDTH}x{height})")


def main() -> None:
	parser = argparse.ArgumentParser(description=__doc__)
	parser.add_argument("--font-dir", type=Path, default=Path("/usr/share/fonts/truetype/dejavu"),
	                    help="directory containing DejaVuSans.ttf and DejaVuSans-Bold.ttf")
	render(parser.parse_args().font_dir)


if __name__ == "__main__":
	main()
