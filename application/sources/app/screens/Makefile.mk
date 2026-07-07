CFLAGS		+= -I./sources/app/screens
CPPFLAGS	+= -I./sources/app/screens

VPATH += sources/app/screens

# CPP source files
SOURCES_CPP += sources/app/screens/screens_bitmap.cpp
SOURCES_CPP += sources/app/screens/scr_idle.cpp
SOURCES_CPP += sources/app/screens/scr_startup.cpp
SOURCES_CPP += sources/app/screens/scr_clock_main.cpp
SOURCES_CPP += sources/app/screens/scr_clock_menu.cpp
SOURCES_CPP += sources/app/screens/scr_clock_setting.cpp
SOURCES_CPP += sources/app/screens/scr_clock_alarm.cpp
SOURCES_CPP += sources/app/screens/scr_clock_stopwatch.cpp
SOURCES_CPP += sources/app/screens/scr_clock_timer.cpp
