#TODO FreeBSD support
CC := clang

COMMON_CFLAGS := -Wall \
-Werror \
-std=c99 \
--pedantic \
--pedantic-errors #\
-fsanitize=undefined \
-fsanitize=address

TARGET := engine
PLATFORMS := linx11 win
linx11_LIBS := -lX11 -lm -lglfw
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

all:
	mkdir -p ./build
	mkdir -p ./build/shaders
	@echo "make win - Windows"
	@echo "make linx11 - Linux, X11 Window System"
	@echo "make obj2kmdl - Model convertor (executable file in /build/models)"
	@echo "make bmp2ktf - Texture convertor (executable file in /build/textures)"
	@echo "make cpshd - Copy shaders from /shaders to /build/shaders"
	@echo "make count - Count lines of code in /src"

cpshd:
	cp ./shaders/* ./build/shaders/

linx11: $(TARGET)-linx11

$(TARGET)-linx11: $(SOURCES)
	$(CC) $(COMMON_CFLAGS) $(INCLUDES) $^ $(linx11_LIBS) -o ./build/$@

win: $(TARGET)-win

$(TARGET)-win: $(SOURCES)
	$(CC) $(COMMON_CFLAGS) $(INCLUDES) $^ $(win_LIBS) -o ./build/$@

obj2kmdl:
	$(CC) $(COMMON_CFLAGS) sdk/obj2kmdl.c -o ./build/models/$@

bmp2ktf:
	$(CC) $(COMMON_CFLAGS) sdk/bmp2ktf.c -o ./build/textures/$@

count:
	cat ./src/* | wc -l
