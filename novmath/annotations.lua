---@meta

---@class go
go = {}

---@class gui
gui = {}

---@class factory
factory = {}

---@class sprite
sprite = {}

---@class physics
physics = {}

---@class camera
camera = {}

---@class render
render = {}

---@class novmath
novmath = {}

---@param url url
---@return number
---@return number
---@return number
function go.get_position_r(url) end

---@param url url
---@param x number
---@param y number
---@param z number
function go.set_position_r(url, x, y, z) end

---@param url url
---@param t_out { x: number, y: number, z: number }
function go.get_position_t(url, t_out) end

---@param url url
---@param t_in { x: number, y: number, z: number }
function go.set_position_t(url, t_in) end

---@param url url
---@return number
function go.get_position_x(url) end

---@param url url
---@param x number
function go.set_position_x(url, x) end

---@param url url
---@return number
function go.get_position_y(url) end

---@param url url
---@param y number
function go.set_position_y(url, y) end

---@param url url
---@return number
function go.get_position_z(url) end

---@param url url
---@param z number
function go.set_position_z(url, z) end

---@param url url
---@return number
---@return number
---@return number
function go.get_scale_r(url) end

---@param url url
---@param x number
---@param y number
---@param z number
function go.set_scale_r(url, x, y, z) end

---@param url url
---@param t_out { x: number, y: number, z: number }
function go.get_scale_t(url, t_out) end

---@param url url
---@param t_in { x: number, y: number, z: number }
function go.set_scale_t(url, t_in) end

---@param url url
---@return number
function go.get_scale_x(url) end

---@param url url
---@param x number
function go.set_scale_x(url, x) end

---@param url url
---@return number
function go.get_scale_y(url) end

---@param url url
---@param y number
function go.set_scale_y(url, y) end

---@param url url
---@return number
function go.get_scale_z(url) end

---@param url url
---@param z number
function go.set_scale_z(url, z) end

---@param url url
---@return number
---@return number
---@return number
function go.get_euler_r(url) end

---@param url url
---@param x number
---@param y number
---@param z number
function go.set_euler_r(url, x, y, z) end

---@param url url
---@param t_out { x: number, y: number, z: number }
function go.get_euler_t(url, t_out) end

---@param url url
---@param t_in { x: number, y: number, z: number }
function go.set_euler_t(url, t_in) end

---@param url url
---@return number
function go.get_euler_x(url) end

---@param url url
---@param x number
function go.set_euler_x(url, x) end

---@param url url
---@return number
function go.get_euler_y(url) end

---@param url url
---@param y number
function go.set_euler_y(url, y) end

---@param url url
---@return number
function go.get_euler_z(url) end

---@param url url
---@param z number
function go.set_euler_z(url, z) end

---@param url url
---@return number
---@return number
---@return number
function go.get_world_position_r(url) end

---@param url url
---@param t_out { x: number, y: number, z: number }
function go.get_world_position_t(url, t_out) end

---@param url url
---@return number
function go.get_world_position_x(url) end

---@param url url
---@return number
function go.get_world_position_y(url) end

---@param url url
---@return number
function go.get_world_position_z(url) end

---@param url url
---@return number
---@return number
---@return number
function go.get_world_scale_r(url) end

---@param url url
---@param t_out { x: number, y: number, z: number }
function go.get_world_scale_t(url, t_out) end

---@param url url
---@return number
function go.get_world_scale_x(url) end

---@param url url
---@return number
function go.get_world_scale_y(url) end

---@param url url
---@return number
function go.get_world_scale_z(url) end

---@param url url
---@return number
---@return number
---@return number
function go.get_world_euler_r(url) end

---@param url url
---@param t_out { x: number, y: number, z: number }
function go.get_world_euler_t(url, t_out) end

---@param url url
---@return number
function go.get_world_euler_x(url) end

---@param url url
---@return number
function go.get_world_euler_y(url) end

---@param url url
---@return number
function go.get_world_euler_z(url) end

---Returns the rotation quaternion as x, y, z, w
---@param url url
---@return number
---@return number
---@return number
---@return number
function go.get_rotation_r(url) end

---@param url url
---@param x number
---@param y number
---@param z number
---@param w number
function go.set_rotation_r(url, x, y, z, w) end

---@param url url
---@param t_out { x: number, y: number, z: number, w: number }
function go.get_rotation_t(url, t_out) end

---@param url url
---@param t_in { x: number, y: number, z: number, w: number }
function go.set_rotation_t(url, t_in) end

---@param url url
---@return number
---@return number
---@return number
---@return number
function go.get_world_rotation_r(url) end

---@param url url
---@param t_out { x: number, y: number, z: number, w: number }
function go.get_world_rotation_t(url, t_out) end

---go.set with 1 (number), 3 (vector3) or 4 (vector4, or quat for "rotation") numbers, followed by optional options
---@param url string|hash|url
---@param property string|hash
---@param x number
---@param y number?
---@param z number?
---@param w number?
---@param options table?
function go.set_r(url, property, x, y, z, w, options) end

---go.set with a table. {x, y, z} is passed as vector3, {x, y, z, w} as vector4.
---"rotation", or a table with `quat = true`, is passed as a quat
---@param url string|hash|url
---@param property string|hash
---@param value number|{ x: number, y: number, z: number, w: number?, quat: boolean? }
---@param options table?
function go.set_t(url, property, value, options) end

---go.get, with vector/quat values returned as separate numbers and matrix4 as 16 numbers.
---Other values (arrays, hashes, ...) are returned with any vmath values converted to plain tables
---@param url string|hash|url
---@param property string|hash
---@param options table?
---@return any ...
function go.get_r(url, property, options) end

---go.get into t_out (16 numbers for a matrix4). Other values are returned as plain values instead
---@param url string|hash|url
---@param property string|hash
---@param t_out { x: number, y: number, z: number, w: number?, quat: boolean? }
---@param options table?
---@return any?
function go.get_t(url, property, t_out, options) end

---go.animate with a table target. Same conversion rules as go.set_t
---@param url string|hash|url
---@param property string|hash
---@param playback constant
---@param to number|{ x: number, y: number, z: number, w: number?, quat: boolean? }
---@param easing constant|vector
---@param duration number
---@param delay number?
---@param complete_function function?
function go.animate_t(url, property, playback, to, easing, duration, delay, complete_function) end

---@param node node
---@return number
---@return number
---@return number
function gui.get_position_r(node) end

---@param node node
---@param x number
---@param y number
---@param z number
function gui.set_position_r(node, x, y, z) end

---@param node node
---@param t_out { x: number, y: number, z: number }
function gui.get_position_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number }
function gui.set_position_t(node, t_in) end

---@param node node
---@return number
function gui.get_position_x(node) end

---@param node node
---@param x number
function gui.set_position_x(node, x) end

---@param node node
---@return number
function gui.get_position_y(node) end

---@param node node
---@param y number
function gui.set_position_y(node, y) end

---@param node node
---@return number
function gui.get_position_z(node) end

---@param node node
---@param z number
function gui.set_position_z(node, z) end

---@param node node
---@return number
---@return number
---@return number
function gui.get_scale_r(node) end

---@param node node
---@param x number
---@param y number
---@param z number
function gui.set_scale_r(node, x, y, z) end

---@param node node
---@param t_out { x: number, y: number, z: number }
function gui.get_scale_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number }
function gui.set_scale_t(node, t_in) end

---@param node node
---@return number
function gui.get_scale_x(node) end

---@param node node
---@param x number
function gui.set_scale_x(node, x) end

---@param node node
---@return number
function gui.get_scale_y(node) end

---@param node node
---@param y number
function gui.set_scale_y(node, y) end

---@param node node
---@return number
function gui.get_scale_z(node) end

---@param node node
---@param z number
function gui.set_scale_z(node, z) end

---@param node node
---@return number
---@return number
---@return number
function gui.get_size_r(node) end

---@param node node
---@param x number
---@param y number
---@param z number
function gui.set_size_r(node, x, y, z) end

---@param node node
---@param t_out { x: number, y: number, z: number }
function gui.get_size_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number }
function gui.set_size_t(node, t_in) end

---@param node node
---@return number
function gui.get_size_x(node) end

---@param node node
---@param x number
function gui.set_size_x(node, x) end

---@param node node
---@return number
function gui.get_size_y(node) end

---@param node node
---@param y number
function gui.set_size_y(node, y) end

---@param node node
---@return number
function gui.get_size_z(node) end

---@param node node
---@param z number
function gui.set_size_z(node, z) end

---@param node node
---@return number
---@return number
---@return number
function gui.get_euler_r(node) end

---@param node node
---@param x number
---@param y number
---@param z number
function gui.set_euler_r(node, x, y, z) end

---@param node node
---@param t_out { x: number, y: number, z: number }
function gui.get_euler_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number }
function gui.set_euler_t(node, t_in) end

---@param node node
---@return number
function gui.get_euler_x(node) end

---@param node node
---@param x number
function gui.set_euler_x(node, x) end

---@param node node
---@return number
function gui.get_euler_y(node) end

---@param node node
---@param y number
function gui.set_euler_y(node, y) end

---@param node node
---@return number
function gui.get_euler_z(node) end

---@param node node
---@param z number
function gui.set_euler_z(node, z) end

---@param node node
---@return number
---@return number
---@return number
---@return number
function gui.get_color_r(node) end

---w is kept when omitted
---@param node node
---@param x number
---@param y number
---@param z number
---@param w number?
function gui.set_color_r(node, x, y, z, w) end

---@param node node
---@param t_out { x: number, y: number, z: number, w: number }
function gui.get_color_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number, w: number?, quat: boolean? }
function gui.set_color_t(node, t_in) end

---@param node node
---@return number
function gui.get_color_x(node) end

---@param node node
---@param x number
function gui.set_color_x(node, x) end

---@param node node
---@return number
function gui.get_color_y(node) end

---@param node node
---@param y number
function gui.set_color_y(node, y) end

---@param node node
---@return number
function gui.get_color_z(node) end

---@param node node
---@param z number
function gui.set_color_z(node, z) end

---@param node node
---@return number
function gui.get_color_w(node) end

---@param node node
---@param w number
function gui.set_color_w(node, w) end

---@param node node
---@return number
---@return number
---@return number
---@return number
function gui.get_outline_r(node) end

---w is kept when omitted
---@param node node
---@param x number
---@param y number
---@param z number
---@param w number?
function gui.set_outline_r(node, x, y, z, w) end

---@param node node
---@param t_out { x: number, y: number, z: number, w: number }
function gui.get_outline_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number, w: number?, quat: boolean? }
function gui.set_outline_t(node, t_in) end

---@param node node
---@return number
function gui.get_outline_x(node) end

---@param node node
---@param x number
function gui.set_outline_x(node, x) end

---@param node node
---@return number
function gui.get_outline_y(node) end

---@param node node
---@param y number
function gui.set_outline_y(node, y) end

---@param node node
---@return number
function gui.get_outline_z(node) end

---@param node node
---@param z number
function gui.set_outline_z(node, z) end

---@param node node
---@return number
function gui.get_outline_w(node) end

---@param node node
---@param w number
function gui.set_outline_w(node, w) end

---@param node node
---@return number
---@return number
---@return number
---@return number
function gui.get_shadow_r(node) end

---w is kept when omitted
---@param node node
---@param x number
---@param y number
---@param z number
---@param w number?
function gui.set_shadow_r(node, x, y, z, w) end

---@param node node
---@param t_out { x: number, y: number, z: number, w: number }
function gui.get_shadow_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number, w: number?, quat: boolean? }
function gui.set_shadow_t(node, t_in) end

---@param node node
---@return number
function gui.get_shadow_x(node) end

---@param node node
---@param x number
function gui.set_shadow_x(node, x) end

---@param node node
---@return number
function gui.get_shadow_y(node) end

---@param node node
---@param y number
function gui.set_shadow_y(node, y) end

---@param node node
---@return number
function gui.get_shadow_z(node) end

---@param node node
---@param z number
function gui.set_shadow_z(node, z) end

---@param node node
---@return number
function gui.get_shadow_w(node) end

---@param node node
---@param w number
function gui.set_shadow_w(node, w) end

---Returns the rotation quaternion as x, y, z, w
---@param node node
---@return number
---@return number
---@return number
---@return number
function gui.get_rotation_r(node) end

---@param node node
---@param x number
---@param y number
---@param z number
---@param w number
function gui.set_rotation_r(node, x, y, z, w) end

---@param node node
---@param t_out { x: number, y: number, z: number, w: number }
function gui.get_rotation_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number, w: number }
function gui.set_rotation_t(node, t_in) end

---@param node node
---@return number
---@return number
---@return number
function gui.get_screen_position_r(node) end

---@param node node
---@param x number
---@param y number
---@param z number
function gui.set_screen_position_r(node, x, y, z) end

---@param node node
---@param t_out { x: number, y: number, z: number }
function gui.get_screen_position_t(node, t_out) end

---@param node node
---@param t_in { x: number, y: number, z: number }
function gui.set_screen_position_t(node, t_in) end

---@param node node
---@param x number
---@param y number
---@param z number
---@return number
---@return number
---@return number
function gui.screen_to_local_r(node, x, y, z) end

---@param node node
---@param t_in { x: number, y: number, z: number }
---@param t_out { x: number, y: number, z: number }
function gui.screen_to_local_t(node, t_in, t_out) end

---@param x number
---@param y number
---@param z number
---@param size_x number
---@param size_y number
---@param size_z number
---@return node
function gui.new_box_node_r(x, y, z, size_x, size_y, size_z) end

---@param position { x: number, y: number, z: number }
---@param size { x: number, y: number, z: number }
---@return node
function gui.new_box_node_t(position, size) end

---@param x number
---@param y number
---@param z number
---@param size_x number
---@param size_y number
---@param size_z number
---@return node
function gui.new_pie_node_r(x, y, z, size_x, size_y, size_z) end

---@param position { x: number, y: number, z: number }
---@param size { x: number, y: number, z: number }
---@return node
function gui.new_pie_node_t(position, size) end

---@param x number
---@param y number
---@param z number
---@param text string
---@return node
function gui.new_text_node_r(x, y, z, text) end

---@param position { x: number, y: number, z: number }
---@param text string
---@return node
function gui.new_text_node_t(position, text) end

---@param x number
---@param y number
---@param z number
---@param particlefx hash|string
---@return node
function gui.new_particlefx_node_r(x, y, z, particlefx) end

---@param position { x: number, y: number, z: number }
---@param particlefx hash|string
---@return node
function gui.new_particlefx_node_t(position, particlefx) end

---gui.set with 1 (number), 3 (vector3) or 4 (vector4, or quat for "rotation") numbers, followed by optional options
---@param node node
---@param property string|hash|constant
---@param x number
---@param y number?
---@param z number?
---@param w number?
---@param options table?
function gui.set_r(node, property, x, y, z, w, options) end

---@param node node
---@param property string|hash|constant
---@param value number|{ x: number, y: number, z: number, w: number?, quat: boolean? }
---@param options table?
function gui.set_t(node, property, value, options) end

---@param node node
---@param property string|hash|constant
---@param options table?
---@return any ...
function gui.get_r(node, property, options) end

---@param node node
---@param property string|hash|constant
---@param t_out { x: number, y: number, z: number, w: number?, quat: boolean? }
---@param options table?
---@return any?
function gui.get_t(node, property, t_out, options) end

---@param node node
---@param property string|hash|constant
---@param to number|{ x: number, y: number, z: number, w: number?, quat: boolean? }
---@param easing constant|vector
---@param duration number
---@param delay number?
---@param complete_function function?
---@param playback constant?
function gui.animate_t(node, property, to, easing, duration, delay, complete_function, playback) end

---@param url string|hash|url
---@param x number
---@param y number
---@param z number
---Table values in properties are converted: {x, y, z} -> vector3, {x, y, z, w} -> vector4, `quat = true` -> quat
---@param properties table?
---@param scale number|{ x: number, y: number, z: number }|nil
---@return hash
function factory.create_r(url, x, y, z, properties, scale) end

---@param url string|hash|url
---@param position { x: number, y: number, z: number }?
---@param rotation { x: number, y: number, z: number, w: number }?
---Table values in properties are converted like in factory.create_r
---@param properties table?
---@param scale number|{ x: number, y: number, z: number }|nil
---@return hash
function factory.create_t(url, position, rotation, properties, scale) end

---@param url string|hash|url
---@param constant string|hash
---@param x number
---@param y number
---@param z number
---@param w number
function sprite.set_constant_r(url, constant, x, y, z, w) end

---@param url string|hash|url
---@param constant string|hash
---@param value { x: number, y: number, z: number, w: number }
function sprite.set_constant_t(url, constant, value) end

---Like physics.raycast, with `position` and `normal` as plain {x, y, z} tables
---@param from_x number
---@param from_y number
---@param from_z number
---@param to_x number
---@param to_y number
---@param to_z number
---@param groups table
---@param options table?
---@return table?
function physics.raycast_r(from_x, from_y, from_z, to_x, to_y, to_z, groups, options) end

---Like physics.raycast, with `position` and `normal` as plain {x, y, z} tables
---@param from { x: number, y: number, z: number }
---@param to { x: number, y: number, z: number }
---@param groups table
---@param options table?
---@return table?
function physics.raycast_t(from, to, groups, options) end

---@param x number
---@param y number
---@param z number
---@param camera url|number|nil
---@return number
---@return number
---@return number
function camera.screen_to_world_r(x, y, z, camera) end

---@param t_in { x: number, y: number, z: number }
---@param t_out { x: number, y: number, z: number }
---@param camera url|number|nil
function camera.screen_to_world_t(t_in, t_out, camera) end

---@param x number
---@param y number
---@param z number
---@param camera url|number|nil
---@return number
---@return number
---@return number
function camera.world_to_screen_r(x, y, z, camera) end

---@param t_in { x: number, y: number, z: number }
---@param t_out { x: number, y: number, z: number }
---@param camera url|number|nil
function camera.world_to_screen_t(t_in, t_out, camera) end

---Matrix is 16 numbers in column-major order
---@param matrix number[]
function render.set_view_t(matrix) end

---Matrix is 16 numbers in column-major order
---@param matrix number[]
function render.set_projection_t(matrix) end

---render.clear where color values are {x, y, z, w} tables
---@param buffers table
function render.clear_t(buffers) end

---render.draw where options.frustum is a table of 16 numbers
---@param predicate userdata
---@param options table?
function render.draw_t(predicate, options) end

---@param constants userdata
---@param name string
---@param x number
---@param y number
---@param z number
---@param w number
function render.set_constant_r(constants, name, x, y, z, w) end

---value is {x, y, z, w} (vector4), 16 numbers (matrix4), or an array of these
---@param constants userdata
---@param name string
---@param value { x: number, y: number, z: number, w: number }|number[]|table[]
function render.set_constant_t(constants, name, value) end

---Like physics.get_shape, with `dimensions` as a plain {x, y, z} table
---@param url string|hash|url
---@param shape string|hash
---@return table
function physics.get_shape_t(url, shape) end

---Writes the camera view matrix into t_out as 16 numbers in column-major order
---@param camera url|number
---@param t_out number[]
function camera.get_view_t(camera, t_out) end

---Writes the camera projection matrix into t_out as 16 numbers in column-major order
---@param camera url|number
---@param t_out number[]
function camera.get_projection_t(camera, t_out) end

---Replaces all vmath values in value with plain tables: vector3 -> {x, y, z},
---vector4/quat -> {x, y, z, w}, matrix4 -> 16 numbers (column-major).
---Tables are converted in place and returned. Use it on engine messages in on_message.
---@generic T
---@param value T
---@return T
function novmath.plain(value) end
