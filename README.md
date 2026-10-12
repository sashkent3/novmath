# NOVMATH

LuaJIT can't compile Lua code that works with vmath values: creating them and reading or writing their fields are all calls into C, so the surrounding code runs in the interpreter. NOVMATH moves all vmath handling into native code. Lua code passes and receives plain numbers and tables, which LuaJIT compiles, and only the engine calls themselves remain.

## Features

### Game Object Transformations

The extension provides comprehensive functions for manipulating game object transforms:

#### Position

- `get_position_r(url)`: Returns x, y, z position components
- `set_position_r(url, x, y, z)`: Sets position using individual components
- `get_position_t(url, t_out)`: Gets position into a table
- `set_position_t(url, t_in)`: Sets position from a table
- Individual component getters/setters for x, y, z

#### Scale

- `get_scale_r(url)`: Returns x, y, z scale components
- `set_scale_r(url, x, y, z)`: Sets scale using individual components
- `get_scale_t(url, t_out)`: Gets scale into a table
- `set_scale_t(url, t_in)`: Sets scale from a table
- Individual component getters/setters for x, y, z

#### Rotation (Euler Angles)

- `get_euler_r(url)`: Returns x, y, z rotation components
- `set_euler_r(url, x, y, z)`: Sets rotation using individual components
- `get_euler_t(url, t_out)`: Gets rotation into a table
- `set_euler_t(url, t_in)`: Sets rotation from a table
- Individual component getters/setters for x, y, z

### World Space Transformations

The extension also provides world space variants of all transform operations:

- World position getters
- World scale getters
- World rotation (euler) getters

#### Rotation (Quaternion)

- `get_rotation_r(url)` / `set_rotation_r(url, x, y, z, w)`
- `get_rotation_t(url, t_out)` / `set_rotation_t(url, t_in)`
- `get_world_rotation_r(url)` / `get_world_rotation_t(url, t_out)`

#### Generic Properties

Forward to `go.set`, `go.get` and `go.animate`:

- `go.set_r(url, property, x, [y, z, [w]], [options])`: 1 number is passed as a number, 3 as a `vector3`, 4 as a `vector4` (a `quat` for `"rotation"`)
- `go.set_t(url, property, value, [options])`: `{x, y, z}` is passed as a `vector3`, `{x, y, z, w}` as a `vector4`. `"rotation"`, or a table with `quat = true`, is passed as a `quat`
- `go.get_r(url, property, [options])`: returns vector components as separate numbers, and a `matrix4` as 16 numbers. Other values, such as array properties, come back with vmath values converted to plain tables
- `go.get_t(url, property, t_out, [options])`

These also work on script properties (`go.get_r("#script", "my_vector")`), which avoids reading `self.my_vector` as a vmath value.
- `go.animate_t(url, property, playback, to, easing, duration, [delay], [complete_function])`

The engine checks property types strictly, so pass `w` for `vector4` properties such as `"tint"`, and `quat = true` for `quat` script properties.

### GUI

Node properties are read and written directly, without going through vmath:

- `position`, `scale`, `size`, `euler`: `gui.get_<prop>_r/_t/_x/_y/_z`, `gui.set_<prop>_r/_t/_x/_y/_z`
- `color`, `outline`, `shadow`: the same, plus `_w`. `w` is kept when omitted from `set_<prop>_r` or `set_<prop>_t`
- `rotation` (quaternion): `gui.get_rotation_r/_t`, `gui.set_rotation_r/_t`
- `gui.get_screen_position_r/_t`, `gui.set_screen_position_r/_t`
- `gui.screen_to_local_r(node, x, y, z)`, `gui.screen_to_local_t(node, t_in, t_out)`
- `gui.new_box_node_r(x, y, z, size_x, size_y, size_z)`, `gui.new_box_node_t(position, size)`, and the same for `new_pie_node`
- `gui.new_text_node_r(x, y, z, text)`, `gui.new_text_node_t(position, text)`, and the same for `new_particlefx_node`
- `gui.set_r`, `gui.set_t`, `gui.get_r`, `gui.get_t`, `gui.animate_t`: same rules as the `go` versions

### Other Modules

- `factory.create_r(url, x, y, z, [properties], [scale])`, `factory.create_t(url, [position], [rotation], [properties], [scale])`: `scale` is a number or `{x, y, z}`. Table values in `properties` are converted like in `go.set_t`; your table is left unchanged
- `sprite.set_constant_r(url, constant, x, y, z, w)`, `sprite.set_constant_t(url, constant, value)`: use `go.set`, since `sprite.set_constant` was removed in Defold 1.13.1
- `physics.raycast_r(from_x, from_y, from_z, to_x, to_y, to_z, groups, [options])`, `physics.raycast_t(from, to, groups, [options])`: `position` and `normal` in the result are plain `{x, y, z}` tables
- `physics.get_shape_t(url, shape)`: like `physics.get_shape`, with `dimensions` as `{x, y, z}`
- `camera.get_view_t(camera, t_out)`, `camera.get_projection_t(camera, t_out)`: 16 numbers in column-major order
- `camera.screen_to_world_r(x, y, z, [camera])`, `camera.screen_to_world_t(t_in, t_out, [camera])`, and the same for `world_to_screen`
- `render.set_view_t(matrix)`, `render.set_projection_t(matrix)`: `matrix` is 16 numbers in column-major order
- `render.clear_t(buffers)`: color values are `{x, y, z, w}`
- `render.draw_t(predicate, [options])`: `options.frustum` is 16 numbers
- `render.set_constant_r(constants, name, x, y, z, w)`, `render.set_constant_t(constants, name, value)`: `value` is `{x, y, z, w}`, 16 numbers, or an array of these

### Engine Messages

Messages from the engine contain vmath values, e.g. `position` and `normal` in `contact_point_response`. `novmath.plain(value)` replaces every vmath value with a plain table (`vector3` → `{x, y, z}`, `vector4`/`quat` → `{x, y, z, w}`, `matrix4` → 16 numbers) in one native call. Tables are converted in place.

```lua
function on_message(self, message_id, message)
	if message_id == hash("contact_point_response") then
		novmath.plain(message)
		local nx, ny = message.normal.x, message.normal.y
	end
end
```

### How It Works

Transforms and GUI node properties use dmSDK directly. Functions without a dmSDK equivalent call the engine function from native code, passing a reused vmath value, and convert any vmath result to numbers or plain tables before returning. Either way, no vmath value reaches Lua. Engine functions are looked up on first use and cached, so replacing e.g. `go.set` from Lua after that point doesn't affect NOVMATH.

Each function is still a call into C, which LuaJIT can't compile. LuaJIT compiles the Lua code around these calls and joins the pieces (trace stitching), so keep engine calls out of the innermost math and work on the returned numbers.

`gui`, `camera` and `render` functions need `script.shared_state = 1` in `game.project` on Defold versions before 1.14. From 1.14, Defold always uses a shared Lua state and the setting no longer exists.

Supported Defold versions: 1.12.1 to 1.14.1.

### Limitations

- `go.property` defaults are parsed at build time and must stay `vmath.vector3(...)`/`vmath.vector4(...)`/`vmath.quat(...)`. Reading such a property as `self.name` gives a vmath value; use `go.get_r("#script", "name")` instead.
- Engine functions that aren't wrapped still return vmath values. Read them through `novmath.plain`, or pass them on to other engine functions without reading their fields.
- Custom easing curves (`vmath.vector`) can't be created natively. Create them once, e.g. in `init`, and pass them as-is.
- `msg.post` sends plain tables as tables. Messages handled by engine components still need vmath values.

## Usage

### Basic Usage

```lua
-- Get position components
local x, y, z = go.get_position_r("/game_object")

-- Set position using components
go.set_position_r("/game_object", 10, 20, 30)

-- Get position into a table
local pos = {x = 0, y = 0, z = 0}
go.get_position_t("/game_object", pos)

-- Set a vector4 property
go.set_r("#sprite", "tint", 1, 1, 1, 0.5)

-- Animate with a table target
go.animate_t(".", "position", go.PLAYBACK_ONCE_FORWARD, {x = 100, y = 0, z = 0}, go.EASING_LINEAR, 1)

-- GUI
gui.set_position_r(node, 10, 20, 0)
gui.set_color_w(node, 0.5)

-- Spawn
local id = factory.create_r("#factory", x, y, 0)
```

### Type Safety

The extension includes Lua type annotations for better IDE support and type checking.

## Installation

```bash
https://github.com/itmurker/novmath/archive/refs/tags/0.1.zip
```

## License

MIT License
