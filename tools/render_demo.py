#!/usr/bin/env python3
"""Render resources/captures/mc_clock_demo_uart.json into the README demo GIF.

Each frame is held for the real time until the next capture (capped at 1.5 s);
the last frame is held 2 s. Pixels are only enlarged (nearest neighbour, x4).

Usage (from the repository root):
  python3 tools/render_demo.py
"""

import json

from render_screens import frame_image

SRC = "resources/captures/mc_clock_demo_uart.json"
OUT = "resources/videos/mini_clock_demo.gif"


def main():
	record = json.load(open(SRC))
	frames = record["frames"]
	images = [frame_image(f["data_hex"], record["width"], record["height"]) for f in frames]
	holds = [min(1500, int((b["t"] - a["t"]) * 1000)) for a, b in zip(frames, frames[1:])] + [2000]
	images[0].save(OUT, save_all=True, append_images=images[1:], duration=holds, loop=0, optimize=False)
	print(f"{OUT}: {len(images)} frames, {sum(holds) / 1000:.1f} s")


if __name__ == "__main__":
	main()
