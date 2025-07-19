# Scene

> **!Warning!**
In the future, scene files will be converted to binary format.

Scenes are in `.ksc` (kvadrat scene) format.
This is a text file.
Scene files contain descriptions of all objects that will be drawn at runtime.
Currently the engine loads only `scenes/example.ksc`.

> Engine uses Opengl coordinate system:
![Coordinate system](./img/scn1.png)
`-Z` - front
`+X` - right
`+Y` - up

## Objects

Each object has parameters.
Parameters are declarated after object keyword.\
Availible objects with parameters:
> (number of repetitions, type (string, float))

* `model`
  * `model_name` (string)
  * `vertex_shader_name` (string)
  * `fragment_shader_name` (string)
  * `texture_name` (string)
  * `coords` (float)
    * `x`
    * `y`
    * `z`
* `light`
  * `position` (float)
    * `x`
    * `y`
    * `z`
  * `color` (float)
    * `x`
    * `y`
    * `z`
  * `constant` (float)
  * `linear` (float)
  * `quadratic` (float)
* `dir_light`
  * `direction` (float)
    * `x`
    * `y`
    * `z`
  * `ambient_color` (float)
    * `x`
    * `y`
    * `z`
  * `color` (float)
    * `x`
    * `y`
    * `z`
* `sector`
  * `bottom` (float)
  * `top` (float)
  * `connection_top` (4, float)
  * `connection_bottom` (4, float)
  * `points` (4, float)
    * `x`
    * `y`
  * `portal` (4, 0-4)
  * `texture_names` (6, string)
  * `vertex_shader_names` (6, string)
  * `fragment_shader_names` (6, string)
  * `texture_coords` (6, float)
    * `x`
    * `y`

See `docs/light.md`, `docs/sector.md`, `docs/model.md`.

## Syntax

Strings cannot contain spaces.\
The decimal point of a floating point number is `.`, not `,`.\
Parameters can be separated into separate lines or placed on one line:

``` text
keyw0rd string1 string2 float3 float4
keyword1
  string5
  string6
  7float float8
```

## Example

``` text
model models/sphere.kmdl shaders/def.vert shaders/def.frag textures/tree.png -1 3 5
model
  models/sphere.kmdl
  shaders/def.vert
  shaders/def.frag
  textures/tree.png
  -1.1234 3.4 5.5
```
