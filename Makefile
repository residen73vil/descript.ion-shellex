CC = g++
WINDRES = windres
CFLAGS = -shared  -DUNICODE -std=c++11
LDFLAGS = -Wl,--kill-at  -luuid -lole32 -lcomctl32 -mwindows -m64

# Debug flags
DEBUG_FLAGS = -D_DEBUG
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
SRCS = dll_main.cpp context_menu.cpp dbg.cpp property_sheet.cpp shellext_init.cpp
RESOURCES = resource.rc
OBJS = $(SRCS:.cpp=.o) $(RESOURCES:.rc=.o)

.PHONY: all clean

all: $(TARGET)

resource.o: $(RESOURCES)
	$(WINDRES) $(RESOURCES) -o $(RESOURCES:.rc=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^  $(LDFLAGS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)