# config.cfg

This file contains the main engine settings.

## Avaliable variables

* `lights_max` - max amount of lights in scene
* `sectors_max` - max amount of sectors in scene
* `vertices_max` - max amount of vertices in model
* `fullscreen` - 0 or 1
* `screen_height` - 128 - 4294967295
* `screen_width` - 128 - 4294967295
* `shader_chars_max` - max size of shader src file in bytes
* `shader_lights_count` - max amount of lights affecting 1 object
* `models_max` - max amount of models in scene
* `texsize_max` - max width or height of texture
* `buf_prog` - max amount of buffered shader programs
* `buf_tex` - max amount of buffered textures
* `buf_vbo` - max amount of buffered models

## Default values

If the `Condition` is true, the engine will display a warning message about this
and set the value from the `Default` column from the following table:

|Value|Condition|Default|
|---|---|---|
|`models_max`        |`value == 0`|`128`|
|`lights_max`        |`value == 0`|`128`|
|`vertices_max`      |`value == 0`|`1024`|
|`screen_height`     |`value < 128`|`128`|
|`screen_width`      |`value < 128`|`128`|
|`shader_chars_max`  |`value == 0`|`4096`|
|`shader_lights_count`|`value == 0`|`4`|
|`sectors_max`       |`value == 0`|`128`|
|`fullscreen`        | - |`0`|
|`texsize_max`       |`value == 0`|`1024`|
|`buf_prog`          |`value == 0`|`32`|
|`buf_tex`           |`value == 0`|`512`|
|`buf_vbo`           |`value == 0`|`512`|

## Syntax

``` text
variable value
```

## Example

``` text
models_max 1024
vertices_max 1024
...
```

