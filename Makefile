#linx11 - linux x11
#win - windows
#obj2kmdl - obj2kmdl util

#linway - linux wayland (glfw does not support wayland)

CC := clang

COMMON_CFLAGS := -Wall \
-Werror \
-std=c99 \
--pedantic \
--pedantic-errors

TARGET := engine

WORKSPACE_FOLDER := .

PLATFORMS := linx11 win

linx11_LIBS := -lX11 -lm -lglfw
#linway_LIBS := -ldrm -lwl_eglplatform -lEGL -lGLESv2
win_LIBS := -lopengl32 -L./lib/GLFW/lib-mingw-w64 -lglfw3dll
INCLUDES := -I./lib/kmatrix/include \
-I./include \
-I./lib/glad/include \
-I./lib/GLFW/include \
-I./lib/stb

SOURCES := $(wildcard ./src/*.c)
SOURCES += $(wildcard ./lib/kmatrix/src/*.c)
SOURCES += ./lib/glad/src/glad.c

.PHONY: cpshd all $(PLATFORMS)

all: $(PLATFORMS)

cpshd:
	cp ./shaders/* ./build/shaders/

linx11: $(TARGET)-linx11

$(TARGET)-linx11: $(SOURCES)
	$(CC) $(COMMON_CFLAGS) $(INCLUDES) $^ $(linx11_LIBS) -o ./build/$@

#linway: $(TARGET)-linway

#$(TARGET)-linway: $(SOURCES)
#	$(CC) $(COMMON_CFLAGS) $(INCLUDES) $^ $(linway_LIBS) -o ./build/$@

win: $(TARGET)-win

$(TARGET)-win: $(SOURCES)
	$(CC) $(COMMON_CFLAGS) $(INCLUDES) $^ $(win_LIBS) -o ./build/$@

obj2kmdl:
	$(CC) $(COMMON_CFLAGS) sdk/obj2kmdl.c -o ./build/models/$@


