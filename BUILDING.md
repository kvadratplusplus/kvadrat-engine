# Build engine

## Windows

**Only Windows 10 64 bit**\
Compiler: mingw clang\
Make: make from gnu

```
make win
cd build
.\engine-win.exe
```

## Linux X11

**Only X11 (GLFW does not support Wayland)**\
Compiler: Clang from repo\
Make: make from repo

```
sudo pacman -S clang glfw make
make linx11
cd build
./engine-linx11
```

