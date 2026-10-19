#!/usr/bin/env python3
"""Render README screen cards (framed) and Objects table previews (unframed).

Framed cards are 720x480 crops of the banner background (tools/render_banner.py)
with a title, captions and a bezel placed outside a 512x256 OLED viewport at
(104, 128). Objects-table previews put the same 512x256 OLED image on a plain
512x288 black canvas. OLED pixels are only enlarged, never edited or tinted.

Usage (from the repository root, after render_screens.py):
  python3 tools/render_readme_cards.py
"""

from PIL import Image, ImageDraw, ImageSequence

from render_banner import BLUE, DIM, FONT_SOURCE, ROOT, WHITE, background, clock_face, label, lettering, parse_array

SCREENS = ROOT / "resources/images/screens"
FRAMED = SCREENS / "framed"
TABLE = ROOT / "resources/images/objects/table"

CARD_W, CARD_H = 720, 480
VIEW_X, VIEW_Y = 104, 128

# name, source preview, top-right caption, subtitle, crop origin in the banner background
CARDS = [
	("scr_clock_main", "scr_clock_main.gif", "DIGITAL", "TIME / WEEKDAY / DATE", (280, 40)),
	("scr_clock_menu", "scr_clock_menu.gif", "MENU", "NINE SCREENS / UP DOWN MODE", (0, 0)),
	("scr_clock_analog", "scr_clock_analog.gif", "ANALOG", "HANDS / DATE / UTC OFFSET", (560, 0)),
	("scr_clock_setting", "scr_clock_setting.png", "SETTING", "TIME / DISPLAY / SOUND / RESET", (140, 160)),
	("scr_clock_ringing", "scr_clock_ringing.gif", "RINGING", "ALARM / TIMER / EVENT", (420, 160)),
]

# name, source preview
TABLE_IMAGES = [
	("mc_clock_rtc", "scr_clock_time_setting.png"),
	("mc_clock_time", "scr_clock_main.gif"),
	("mc_clock_alarm", "scr_clock_alarm_list.png"),
	("mc_clock_stopwatch", "scr_clock_stopwatch.gif"),
	("mc_clock_timer", "scr_clock_timer.gif"),
	("mc_clock_world", "scr_clock_world.gif"),
	("mc_clock_calendar", "scr_clock_calendar_month.png"),
	("mc_clock_ringing", "scr_clock_ringing.gif"),
]


def load(path):
	with Image.open(path) as image:
		return [(frame.convert("RGB"), frame.info.get("duration", 1000)) for frame in ImageSequence.Iterator(image)]


def save(frames, path):
	"""PNG for one frame; GIF keeps the source order, holds and looping."""
	if len(frames) == 1:
		frames[0][0].save(path.with_suffix(".png"), optimize=True)
		return path.with_suffix(".png")
	palette = frames[0][0].quantize(colors=128, dither=Image.NONE)
	encoded = [image.quantize(palette=palette, dither=Image.NONE) for image, _ in frames]
	encoded[0].save(path.with_suffix(".gif"), save_all=True, append_images=encoded[1:],
	                duration=[d for _, d in frames], loop=0, disposal=2, optimize=False)
	return path.with_suffix(".gif")


def card_background(font, caption, subtitle, origin, number):
	base = background()
	clock_face(ImageDraw.Draw(base), 0)
	x, y = origin
	card = base.crop((x, y, x + CARD_W, y + CARD_H))
	draw = ImageDraw.Draw(card)
	card.paste(WHITE, (40, 38), lettering(font, "MINI CLOCK", 3))
	right = lettering(font, caption, 2)
	card.paste(BLUE, (CARD_W - 40 - right.width, 42), right)
	card.paste(DIM, (40, 82), lettering(font, subtitle, 2))
	draw.rectangle((VIEW_X - 14, VIEW_Y - 14, VIEW_X + 512 + 13, VIEW_Y + 256 + 13), outline=BLUE, width=3)
	draw.rectangle((VIEW_X - 6, VIEW_Y - 6, VIEW_X + 512 + 5, VIEW_Y + 256 + 5), fill=(2, 4, 8), outline=(40, 78, 130), width=2)
	card.paste(DIM, (40, 432), lettering(font, "AK BASE KIT / 128X64 OLED", 2))
	footer = lettering(font, f"SCREEN {number:02d}", 2)
	card.paste(BLUE, (CARD_W - 40 - footer.width, 432), footer)
	return card


def main():
	font = parse_array(FONT_SOURCE, "font")
	(FRAMED / "backgrounds").mkdir(parents=True, exist_ok=True)
	TABLE.mkdir(parents=True, exist_ok=True)

	for number, (name, source, caption, subtitle, origin) in enumerate(CARDS, 1):
		card = card_background(font, caption, subtitle, origin, number)
		card.save(FRAMED / "backgrounds" / f"{name}.png", optimize=True)
		frames = []
		for oled, duration in load(SCREENS / source):
			frame = card.copy()
			frame.paste(oled, (VIEW_X, VIEW_Y))
			frames.append((frame, duration))
		out = save(frames, FRAMED / name)
		print(f"framed  {out.relative_to(ROOT)}  ({len(frames)} frame(s))")

	for name, source in TABLE_IMAGES:
		frames = []
		for oled, duration in load(SCREENS / source):
			frame = Image.new("RGB", (512, 288))
			frame.paste(oled, (0, 16))
			frames.append((frame, duration))
		out = save(frames, TABLE / name)
		print(f"table   {out.relative_to(ROOT)}  ({len(frames)} frame(s))")


if __name__ == "__main__":
	main()
