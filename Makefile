CC = g++
CFLAGS = -shared  -DUNICODE -std=c++11
LDFLAGS = -Wl,--kill-at  -luuid -lole32

# Debug flags
DEBUG_FLAGS = -D_DEBUG -g
# Release flags
RELEASE_FLAGS =

# Set default build type to 'debug' if not overridden externally
BUILD_TYPE ?= debug

ifeq ($(BUILD_TYPE),debug)
    CFLAGS += $(DEBUG_FLAGS)
else
    CFLAGS += $(RELEASE_FLAGS)
endif


TARGET = descript.ion-shellex.dll
SRCS = dll_main.cpp context_menu.cpp dbg.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^  $(LDFLAGS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)