CFLAGS		+= -I./sources/app/mini_clock
CPPFLAGS	+= -I./sources/app/mini_clock

VPATH += sources/app/mini_clock

SOURCES_CPP += sources/app/mini_clock/mc_clock_time.cpp
SOURCES_CPP += sources/app/mini_clock/mc_clock_stopwatch.cpp
SOURCES_CPP += sources/app/mini_clock/mc_clock_timer.cpp
SOURCES_CPP += sources/app/mini_clock/mc_clock_alarm.cpp
