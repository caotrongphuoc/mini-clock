#!/usr/bin/env python3
"""Capture every Mini Clock screen from a real kit over the UART shell.

Drives the clock with the `btn` shell command, reads the OLED framebuffer with
`lcd d`, and writes resources/captures/mc_clock_screens_uart.json. The firmware
must be a diagnostic build with LOGIN_PRINT_EN and SYS_PRINT_EN enabled.

The run adds sample alarms and a calendar event, changes the RTC to trigger a
ringing page, and removes / restores all of that at the end.

Usage (from the repository root, clock on the Digital screen):
  python3 tools/capture_screens.py --port /dev/ttyUSB0
"""

import argparse
import datetime
import json
import re
import sys
import time
import zoneinfo

import serial

VN = zoneinfo.ZoneInfo("Asia/Ho_Chi_Minh")


class Kit:
	def __init__(self, port, out):
		self.p = serial.Serial(port, 115200, timeout=0.02)
		time.sleep(0.2)
		self.frames = []
		self.out = out
		self.checksum = "unknown"
		self.section = ""
		self.complete = False

	def cmd(self, text, until=None, wait=3.0):
		self.p.reset_input_buffer()
		self.p.write(text.encode() + b"\r")
		buf = b""
		start = time.time()
		while time.time() - start < wait:
			buf += self.p.read(4096)
			if until and until(buf):
				break
		return buf.decode(errors="replace").replace("\r", "")

	def press(self, keys, gap=0.6):
		"""Send each `btn <key>` once; stop if the shell did not run it."""
		for k in keys:
			out = self.cmd("btn " + k, until=lambda b, k=k: ("\nbtn " + k + "\n").encode() in b.replace(b"\r", b""), wait=3.0)
			if "\nbtn " + k + "\n" not in out:
				sys.exit(f"button '{k}' was not acknowledged; check the kit and rerun")
			time.sleep(gap)

	def menu(self, index):
		"""From the Digital screen: open the menu and enter item `index`."""
		self.press("m", 1.5)
		for _ in range(index):
			self.press("u", 1.5)             # presses during the slide animation are ignored
		self.press("m", 1.5)

	def stt(self):
		return self.cmd("stt", until=lambda b: re.search(rb"calendar total.*ring:\d", b) is not None, wait=2.0)

	def lcd(self):
		"""Read the framebuffer; retry when a busy handler (EEPROM write) cut the reply."""
		for attempt in range(3):
			out = self.cmd("lcd d", until=lambda b: b"=> end" in b, wait=8.0)
			if "bytes=1024" in out and "=> end" in out:
				body = out.split("bytes=1024", 1)[1].split("=> end", 1)[0]
				data = [int(x, 16) for x in re.findall(r"0x[0-9A-Fa-f]{2}", body)]
				if len(data) == 1024:
					return bytes(data).hex()
			time.sleep(1.0)
		self.save()
		sys.exit("lcd d failed 3 times; partial capture saved")

	def grab(self, screen, label, hold_ms, count=1, every=0.0):
		for i in range(count):
			if i:
				time.sleep(every)
			self.frames.append({"section": self.section, "screen": screen, "label": label, "duration_ms": hold_ms,
			                    "captured_at": datetime.datetime.now(VN).strftime("%H:%M:%S"),
			                    "data_hex": self.lcd()})
			print(f"  {screen:24s} {label}")
		self.save()

	def save(self):
		"""Write what has been captured so far (the file is complete after cleanup)."""
		record = {
			"format": "ak-oled-page-major-lsb",
			"width": 128,
			"height": 64,
			"firmware_checksum": self.checksum,
			"captured_date": datetime.datetime.now(VN).strftime("%Y-%m-%d"),
			"complete": self.complete,
			"frames": self.frames,
		}
		with open(self.out, "w") as f:
			json.dump(record, f, indent=2)
			f.write("\n")

	def rtc_set_now(self, offset_s=0.0):
		time.sleep(1 - (time.time() % 1) + 0.005)
		now = datetime.datetime.now(VN) + datetime.timedelta(seconds=offset_s)
		self.cmd(now.strftime("rtc s %Y%m%d %H%M%S"), wait=1.0)


SECTIONS = ["Digital", "Menu", "Idle", "Analog", "Alarm", "Stopwatch", "Timer", "World Clock",
            "Calendar", "Setting", "Ringing", "Cleanup"]


def main():
	ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	ap.add_argument("--port", default="/dev/ttyUSB0")
	ap.add_argument("--out", default="resources/captures/mc_clock_screens_uart.json")
	ap.add_argument("--from", dest="start", choices=SECTIONS, default="Digital",
	                help="resume at this section, keeping the frames already saved before it "
	                     "(the kit must be on the Digital screen with the sample alarms present)")
	args = ap.parse_args()

	k = Kit(args.port, args.out)
	ver = k.cmd("ver", until=lambda b: b"console:" in b)
	k.checksum = "0x" + re.search(r"Firmware checksum: (\w+)", ver).group(1).upper()
	state = k.stt()

	first = SECTIONS.index(args.start)
	if first == 0:
		if "alarm total:0" not in state or "calendar total:0" not in state:
			sys.exit("start with no alarms and no events so the cleanup restores the kit:\n" + state)
	else:
		saved = json.load(open(args.out))
		if saved["firmware_checksum"] != k.checksum:
			sys.exit(f"saved frames come from {saved['firmware_checksum']}, kit runs {k.checksum}")
		k.frames = [f for f in saved["frames"] if f["section"] in SECTIONS[:first]]

	def section(name):
		if SECTIONS.index(name) < first:
			return False
		print(name)
		k.section = name
		return True

	if section("Digital"):
		k.grab("scr_clock_main", "digital", 1000, 3, 0.4)

	if section("Menu"):
		k.press("m", 1.2)
		names = ["Digital", "Analog", "Alarm", "Stopwatch", "Timer", "World Clock", "Calendar", "Setting", "Exit"]
		k.grab("scr_clock_menu", names[0], 900)
		for name in names[1:]:
			k.press("u", 1.0)
			k.grab("scr_clock_menu", name, 900)
		k.press("m", 1.0)                  # Exit -> idle (the next section starts here)

	if section("Idle"):
		k.press("uu", 0.4)
		k.grab("scr_idle", "balls", 300, 4, 0.0)
		k.press("m", 1.2)                  # back to the menu
		k.press("m", 1.0)                  # Digital

	if section("Analog"):
		k.menu(1)
		k.grab("scr_clock_analog", "analog", 1000, 3, 0.4)
		k.press("m", 1.2); k.press("m", 1.0)

	if section("Alarm"):
		k.menu(2)
		k.press("m", 1.0); k.press("m"); k.press("dd"); k.press("MMMMM", 0.8)            # 06:00 on
		k.press("d"); k.press("m", 1.0); k.press("m"); k.press("d"); k.press("MMMMM", 0.8); k.press("U", 1.0)  # 07:00 off
		k.press("d"); k.press("m", 1.0); k.press("m"); k.press("u"); k.press("MMMMM", 0.8)  # 09:00 on
		k.press("uu")
		print(k.stt().splitlines()[1:5])
		k.grab("scr_clock_alarm", "list", 2000)
		k.press("m", 0.8)
		k.grab("scr_clock_alarm", "edit", 2000)
		k.press("b", 1.2)                  # leave mid-edit (discarded on the next entry)
		k.press("m", 1.0)

	if section("Stopwatch"):
		k.menu(3)
		k.grab("scr_clock_stopwatch", "help", 1500)
		time.sleep(5.0)
		k.press("m", 1.0); k.press("u", 0.9); k.press("u", 0.5)
		k.grab("scr_clock_stopwatch", "running", 1000, 3, 0.2)
		k.press("m", 0.5); k.press("M", 0.5); k.press("U", 1.2); k.press("m", 1.0)

	if section("Timer"):
		k.menu(4)
		time.sleep(5.2)
		k.grab("scr_clock_timer", "set", 1500)
		k.press("m", 0.6)
		k.grab("scr_clock_timer", "running", 1000, 3, 0.2)
		k.press("m", 0.5); k.press("U", 0.5); k.press("D", 1.2); k.press("m", 1.0)

	if section("World Clock"):
		k.menu(5)
		k.grab("scr_clock_world", "Vietnam", 1500)
		k.press("u", 1.2); k.grab("scr_clock_world", "England", 1500)
		k.press("uu", 1.2); k.grab("scr_clock_world", "France", 1500)
		k.press("uu", 1.2); k.grab("scr_clock_world", "Egypt", 1500)
		k.press("ddddd", 1.0)              # back to Vietnam (each change saves the setting)
		k.press("m", 1.2); k.press("m", 1.0)

	if section("Calendar"):
		k.menu(6)
		k.grab("scr_clock_calendar", "month", 2000)
		k.press("M", 0.8)
		k.grab("scr_clock_calendar", "edit", 2000)
		k.press("mmmmmmmmm", 0.5)
		time.sleep(1.5)                    # the last field saves the bank to EEPROM
		k.grab("scr_clock_calendar", "month_event", 2000)
		k.press("m", 0.8)
		k.grab("scr_clock_calendar", "day", 2000)
		k.press("M", 2.0)                  # delete the sample event (EEPROM write)
		k.press("a", 1.2); k.press("m", 1.0)

	if section("Setting"):
		k.menu(7)
		k.grab("scr_clock_setting", "list", 2000)
		k.press("m", 0.8)
		k.grab("scr_clock_time_setting", "time", 2000)
		k.press("mmmmmmmm", 0.4)           # walk to OK and save (RTC is re-synced below)
		time.sleep(1.0)
		k.press("d"); k.press("m", 0.8)
		k.grab("scr_clock_setting_display", "display", 2000)
		k.press("ddd"); k.press("m", 1.0)
		k.press("dd"); k.press("m", 0.8)
		k.grab("scr_clock_setting_sound", "sound", 2000)
		k.press("ddd"); k.press("m", 1.0)
		k.press("dddd"); k.press("m", 1.2); k.press("m", 1.0)

	if section("Ringing"):
		time.sleep(1 - (time.time() % 1) + 0.005)
		k.cmd(datetime.datetime.now(VN).strftime("rtc s %Y%m%d 055957"), wait=1.0)
		time.sleep(4.5)
		k.grab("scr_clock_ringing", "alarm", 1000, 2, 0.1)
		k.press("m", 1.2)

	if section("Cleanup"):
		k.menu(2)
		k.press("DDD", 1.2)
		k.press("b", 1.2); k.press("m", 1.0)
		k.rtc_set_now()
		state = k.stt()
		print(state)
		if "alarm total:0" not in state or "calendar total:0" not in state:
			print("WARNING: sample data left on the kit")

	k.complete = True
	k.save()
	print(f"{len(k.frames)} frames -> {args.out}")


if __name__ == "__main__":
	main()
