# Mini Clock - Coding Rules and Development Guide

## 1. Build and Flash

The application builds with the GNU Arm toolchain 10.3-2021.10. By default the Makefile looks for it in `~/workspace/tools` (`WORKSPACE_TOOLS_PATH ?= ../../../tools`); override `GCC_PATH` or `WORKSPACE_TOOLS_PATH` for another location.

```bash
make -C application clean
make -C application                              # build, prints Flash and RAM usage
make -C application flash dev=/dev/ttyUSB0       # flash through the AK bootloader (ak-flash)
```

The application starts at `0x08003000`, after the AK bootloader and the shared boot partition.

**Memory** (release build): Flash 74,728 / 118,784 bytes, RAM 13,324 / 16,384 bytes. About 3 KB of RAM is left for the stack, so keep stack copies small and put constant tables in flash (`static const`).

## 2. UART Debug Shell

The console runs at **115200 8N1**. `tools/ak-console.py` sends commands and prints the replies without an interactive terminal:

```bash
python3 tools/ak-console.py --list
python3 tools/ak-console.py --port /dev/ttyUSB0 --cmd "ver" --cmd "fatal l"
python3 tools/ak-console.py --port /dev/ttyUSB0 --watch 10
```

| Command | Purpose |
| --- | --- |
| `ver` | Kernel and app version, firmware checksum, stack and heap available. |
| `fatal l` | Last FATAL record and restart counters. |
| `ram s` | Stack size and peak usage. |
| `lcd d` | OLED framebuffer dump (128×64, page-major). |
| `stt` | Alarm and calendar state (list position, edit mode, ringing). |
| `rtc g` / `rtc s YYYYMMDD hhmmss` | Read or set the RTC (Vietnam time). |
| `mute 1` / `mute 0` | Saved mute setting, without the click sounds of the menu. |
| `btn m\|u\|d` | Simulate a press of Mode, Up, or Down. |
| `btn M\|U\|D` | Simulate a long press. |
| `btn a\|b` | Simulate Mode+Up or Mode+Down. |

### Console output gates

The release build compiles the console output out in `application/Makefile`:

```make
CONSOLE_OPTION +=			\
		-ULOGIN_PRINT_EN	\
		-USYS_PRINT_EN		\
		...
```

Commands still run, but print nothing; the shell only echoes what you type. For a debugging session, change `-ULOGIN_PRINT_EN` and `-USYS_PRINT_EN` to `-D`, rebuild and flash, then restore `-U` afterwards. Keep `LOG_AK_KERNEL_ENABLE` in `application/sources/ak/ak.cfg.mk` off unless you are profiling: it prints every message and starves the shell.
