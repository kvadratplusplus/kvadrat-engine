# config.cfg

This file contains the main engine settings.

## Avaliable variables

* `lights_max` - max amount of lights in scene
* `sectors_max` - max amount of sectors in scene
* `vertices_max` - max amount of vertices in model
* `fullscreen`
* `screen_height`
* `screen_width`
* `shader_chars_max` - max size of shader src file in bytes
* `shader_lights_count` - max amount of lights affecting 1 object
* `models_max` - max amount of models in scene

## Default values

If the value is not set or is 0, the engine will display a warning message about this and set the value from the `If not set` column from the following table:

|Value|Default|If not set|
|---|---|---|
|`shader_chars_max`|`4096`|`4096`|
|`vertices_max`|`1024`|`1024`|
|`lights_max`|`1024`|`128`|
|`sectors_max`|`1024`|`128`|
|`models_max`|`1024`|`128`|
|`screen_width`|`1280`|`128`|
|`screen_height`|`720`|`128`|
|`shader_lights_count`|`4`|`4`|
|`fullscreen`|`0`|`0`|

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
