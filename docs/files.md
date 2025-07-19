# File structure

## Required files

``` text
folder
|-engine.exe
|-glfw3.dll
|-binds.txt
|-config.cfg
|-scenes/
| |-example.ksc
|-models/
| |-missing.kmdl
| |-obj2kmdl.exe
|-textures/
| |-missing.png
|-shaders/
| |-missing.vert
| |-missing.frag
```

These files are required to run the engine. These are the paths it looks for them in.

## engine.exe

The engine executable.
You can add `-log` to view log messages in the `log.log` file.
See `docs/log.md`.

## glfw3.dll

GLFW library file. GLFW handles window and input.

## binds.txt

See `docs/binds.md`.

## config.cfg

See `docs/config.md`.

## models/

See `docs/models.md`.

## textures/

See `docs/textures.md`.

## shaders/

See `docs/shaders.md`.

## scenes/

See `docs/scene.md`.
