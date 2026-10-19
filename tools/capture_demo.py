#!/usr/bin/env python3
"""Record the README demo: a scripted tour of the clock captured from the kit's OLED.

Tour: Digital -> Menu -> Analog -> World Clock -> Stopwatch -> add an alarm ->
alarm rings -> dismiss -> Digital. Frames are read with `lcd d` (about one per
second over UART) while `btn` presses drive the clock. Each frame keeps its real
capture time; render_demo.py uses the gaps as GIF holds.

Needs the diagnostic build (LOGIN_PRINT_EN and SYS_PRINT_EN on), the clock on the
Digital screen with no alarms. The sample alarm is deleted and the RTC re-synced
to Vietnam time afterwards.

Usage (from the repository root):
  python3 tools/capture_demo.py --port /dev/ttyUSB0
"""

import argparse
import datetime
import json
import re
import sys
import time

from capture_screens import VN, Kit


class Recorder(Kit):
	def snap(self, label, count=1):
		for _ in range(count):
			self.frames.append({"label": label, "t": round(time.time(), 3), "data_hex": self.lcd()})
			print(f"  {len(self.frames):3d} {label}")

	def press_snap(self, key, label, count=1, gap=0.15):
		self.press(key, gap)
		self.snap(label, count)


def main():
	ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	ap.add_argument("--port", default="/dev/ttyUSB0")
	ap.add_argument("--out", default="resources/captures/mc_clock_demo_uart.json")
	args = ap.parse_args()

	k = Recorder(args.port, "/dev/null")
	ver = k.cmd("ver", until=lambda b: b"console:" in b)
	checksum = "0x" + re.search(r"Firmware checksum: (\w+)", ver).group(1).upper()
	state = k.stt()
	if "alarm total:0" not in state:
		sys.exit("start on the Digital screen with no alarms:\n" + state)

	k.snap("digital", 3)
	k.press_snap("m", "menu", 1, 0.6)
	k.press_snap("u", "menu analog", 1, 0.6)
	k.press_snap("m", "analog", 3, 0.4)
	k.press_snap("m", "menu", 1, 0.8)
	for item in ("analog", "alarm", "stopwatch", "timer", "world clock"):
		k.press_snap("u", "menu " + item, 1, 0.6)
	k.press_snap("m", "world vietnam", 2, 0.4)
	k.press_snap("u", "world england", 2, 0.6)
	k.press_snap("d", "world vietnam", 1, 0.6)
	k.press_snap("m", "menu", 1, 0.8)
	for item in ("analog", "alarm", "stopwatch"):
		k.press_snap("u", "menu " + item, 1, 0.6)
	k.press_snap("m", "stopwatch help", 1, 0.4)
	k.press_snap("m", "stopwatch running", 2, 0.2)
	k.press_snap("u", "stopwatch lap", 2, 0.2)
	k.press_snap("m", "stopwatch paused", 1, 0.2)
	k.press("M", 0.4)
	k.press("U", 1.2)
	k.press_snap("u", "menu analog", 1, 0.6)
	k.press_snap("u", "menu alarm", 1, 0.6)
	k.press_snap("m", "alarm empty", 1, 0.6)
	k.press_snap("m", "alarm added", 1, 0.8)
	if "alarm total:1" not in k.stt():
		sys.exit("the sample alarm was not added")
	k.press("b", 1.2)
	k.press_snap("m", "digital", 1, 0.8)

	time.sleep(1 - (time.time() % 1) + 0.005)
	k.cmd(datetime.datetime.now(VN).strftime("rtc s %Y%m%d 075955"), until=lambda b: b"rtc set" in b, wait=2.0)
	for _ in range(12):                    # until the alarm task reports ringing
		k.snap("digital 07:59")
		if "ring:1" in k.stt():
			break
	else:
		sys.exit("the alarm did not ring")
	k.snap("alarm ringing", 3)
	k.press_snap("m", "dismissed", 2, 1.2)
	if "ring:0" not in k.stt():
		sys.exit("the alarm was not dismissed")

	# cleanup (not recorded)
	k.menu(2)
	k.press("D", 1.5)
	k.press("b", 1.5)
	k.press("m", 1.0)
	k.rtc_set_now()
	state = k.stt()
	print(state)
	if "alarm total:0" not in state:
		print("WARNING: sample alarm left on the kit")

	with open(args.out, "w") as f:
		json.dump({"format": "ak-oled-page-major-lsb", "width": 128, "height": 64,
		           "firmware_checksum": checksum,
		           "captured_date": datetime.datetime.now(VN).strftime("%Y-%m-%d"),
		           "complete": True, "frames": k.frames}, f, indent=2)
		f.write("\n")
	print(f"{len(k.frames)} frames -> {args.out}")


if __name__ == "__main__":
	main()
