CC=gcc
CFLAGS=-Wall -I/opt/homebrew/opt/sdl2/include/SDL2  # 更新 include 路徑
LDFLAGS=-L/opt/homebrew/opt/sdl2/lib               # 更新 lib 路徑
LIBS=-lSDL2

# Detect the operating system
ifeq ($(OS),Windows_NT)
    CC=i686-w64-mingw32-gcc
    LDFLAGS=-Llib
    LIBS=-lmingw32 -lSDL2main $(LIBS) -mwindows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAGS+=-D_REENTRANT
    endif
    ifeq ($(UNAME_S),Darwin)
        CFLAGS+=-D_THREAD_SAFE
    endif
endif

TARGET=program
SRC=main.c
OBJ=$(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
