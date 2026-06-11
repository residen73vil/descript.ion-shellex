CC = g++
WINDRES = windres
CFLAGS = -shared  -DUNICODE -D_UNICODE -DWINVER=0x0600 -D_WIN32_WINNT=0x0600 -D_WIN32_IE=0x0600 -U__USE_MINGW_ANSI_STDIO -fno-exceptions -fno-rtti -O2 -s -fno-threadsafe-statics
LDFLAGS = -Wl,--kill-at -Wl,--subsystem,windows -Wl,--major-subsystem-version,6 -Wl,--minor-subsystem-version,0 -luuid -loleaut32 -lole32 -lcomctl32 -mwindows -lucrt  -lmsvcrt 

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
SRCS = dll_main.cpp context_menu.cpp dbg.cpp property_sheet.cpp shellext_init.cpp description_handler.cpp \
		description_file_rw.cpp file_io_wrapper.cpp  errors_and_settings.cpp bom_cp_eol_utils.cpp \
		column_provider.cpp
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
