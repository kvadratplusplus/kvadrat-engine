# Light

Light sources can emit different colors and have different light attenuation formulas.
There is also a directional light that attenuates on all objects in the scene.

## Scene object

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

`color` - a 3D vector of values from 0.0 to 1.0, represented in RGB format.

### `light`

`position` - a 3D vector representing the position of the light source in the scene.\
`constant`, `linear` and `quadratic` are the three variables used in the slightly modified constant-linear-quadratic falloff formula.

See `docs/shaders.md`.

### `dir_light`

`direction` - a three-dimensional vector representing the direction in which sunlight falls.\
`ambient_color` - a 3D vector of values from 0.0 to 1.0, represented in RGB format. Applied to all objects in scene.
