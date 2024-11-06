CC = g++
CFLAGS = -shared  -DUNICODE -std=c++11
LDFLAGS = -Wl,--kill-at  -luuid -lole32

TARGET = descript.ion-shellex.dll
SRCS = dll_main.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^  $(LDFLAGS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)