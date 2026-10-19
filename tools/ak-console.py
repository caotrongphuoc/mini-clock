#!/usr/bin/env python3
"""Non-interactive AK UART console helper (115200 8N1).

Examples:
  python3 tools/ak-console.py --list
  python3 tools/ak-console.py --port /dev/ttyUSB0 --cmd "ver" --cmd "fatal l"
  python3 tools/ak-console.py --port /dev/ttyUSB0 --watch 15
  python3 tools/ak-console.py --port /dev/ttyUSB0 --key f
"""

import argparse
import sys
import time

try:
	import serial
	import serial.tools.list_ports
except ImportError:
	sys.exit("pyserial is required: pip install pyserial")


def read_until_quiet(port, quiet_s, max_s):
	"""Read until no byte arrives for quiet_s seconds, or max_s elapses."""
	data = bytearray()
	start = time.monotonic()
	last = start
	while time.monotonic() - start < max_s:
		chunk = port.read(4096)
		if chunk:
			data += chunk
			last = time.monotonic()
		elif time.monotonic() - last >= quiet_s:
			break
	return data.decode(errors="replace")


def main():
	parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser.add_argument("--port", help="serial port, e.g. /dev/ttyUSB0")
	parser.add_argument("--baud", type=int, default=115200)
	parser.add_argument("--cmd", action="append", default=[], help="shell command (repeatable)")
	parser.add_argument("--key", action="append", default=[], help="single key without CR (fatal mode)")
	parser.add_argument("--watch", type=float, help="only listen for N seconds")
	parser.add_argument("--quiet", type=float, default=0.8, help="silence that ends a reply (s)")
	parser.add_argument("--timeout", type=float, default=10.0, help="max wait per command (s)")
	parser.add_argument("--list", action="store_true", help="list serial ports")
	args = parser.parse_args()

	if args.list:
		for p in serial.tools.list_ports.comports():
			if p.description != "n/a":
				print(f"{p.device}\t{p.description}")
		return

	if not args.port:
		parser.error("--port is required")

	with serial.Serial(args.port, args.baud, timeout=0.05) as port:
		time.sleep(0.2)
		port.reset_input_buffer()

		if args.watch:
			print(read_until_quiet(port, args.watch, args.watch), end="")
			return

		for key in args.key:
			port.write(key.encode())
			print(f"===== key {key} =====")
			print(read_until_quiet(port, args.quiet, args.timeout))

		for cmd in args.cmd:
			port.write(cmd.encode() + b"\r")
			print(f"===== {cmd} =====")
			print(read_until_quiet(port, args.quiet, args.timeout))


if __name__ == "__main__":
	main()
