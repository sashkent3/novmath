#include <dmsdk/sdk.h>
#include "novmath_common.h"

// These functions have no dmSDK equivalent, so they forward to the engine.

// Factory

// Replaces the properties table at index with a copy where table values are vmath.
// The engine reads all properties at once, so each gets its own vmath value.
static void ConvertProperties(lua_State *L, int index)
{
    if (!lua_istable(L, index))
        return;
    int count = 0;
    bool has_tables = false;
    lua_pushnil(L);
    while (lua_next(L, index) != 0)
    {
        ++count;
        has_tables |= lua_istable(L, -1);
        lua_pop(L, 1);
    }
    if (!has_tables)
        return;

    lua_createtable(L, 0, count);
    int properties = lua_gettop(L);
    lua_pushnil(L);
    while (lua_next(L, index) != 0)
    {
        int value = lua_gettop(L);
        lua_pushvalue(L, value - 1);
        if (lua_istable(L, value))
        {
            NovmathValue v;
            luaL_argcheck(L, NovmathReadTable(L, value, false, &v) != KIND_OTHER, index, "property tables must be {x, y, z, [w]}");
            NovmathPushValue(L, v, -1);
        }
        else
        {
            lua_pushvalue(L, value);
        }
        lua_rawset(L, properties);
        lua_pop(L, 1);
    }
    lua_replace(L, index);
}

// Stack: url, position, rotation, properties, scale
static int FactoryCreate(lua_State *L)
{
    ConvertProperties(L, 4);
    NovmathConvertArg(L, 5, 1, false);
    NovmathCall(L, ENGINE_FACTORY_CREATE, 1, 1);
    return 1;
}

// factory.create_r(url, x, y, z, [properties], [scale])
static int FACTORY_CreateR(lua_State *L)
{
    dmVMath::Vector3 position = CheckV3R(L, 2);
    lua_settop(L, 6);
    NovmathPushVector3(L, position, 0);
    lua_replace(L, 2);
    lua_pushnil(L); // rotation
    lua_replace(L, 3);
    lua_remove(L, 4);
    return FactoryCreate(L);
}

// factory.create_t(url, [position], [rotation], [properties], [scale])
static int FACTORY_CreateT(lua_State *L)
{
    lua_settop(L, 5);
    NovmathConvertArg(L, 2, 0, false);
    NovmathConvertArg(L, 3, 0, true);
    return FactoryCreate(L);
}

// Sprite. sprite.set_constant was removed in Defold 1.13.1, so these use go.set,
// which is what it was replaced with.

static int SpriteSetConstant(lua_State *L, const dmVMath::Vector4 &v)
{
    lua_settop(L, 2);
    NovmathPushVector4(L, v, 0);
    NovmathCall(L, ENGINE_GO_SET, 1, 0);
    return 0;
}

// sprite.set_constant_r(url, constant, x, y, z, w)
static int SPRITE_SetConstantR(lua_State *L) { return SpriteSetConstant(L, CheckV4R(L, 3)); }
// sprite.set_constant_t(url, constant, {x, y, z, w})
static int SPRITE_SetConstantT(lua_State *L) { return SpriteSetConstant(L, CheckV4TStrict(L, 3)); }

// Physics. Results have vmath values converted to plain tables.

static int Raycast(lua_State *L, const dmVMath::Vector3 &from, const dmVMath::Vector3 &to, int groups_index)
{
    lua_settop(L, groups_index + 1);
    int base = lua_gettop(L) + 1;
    NovmathPushVector3(L, from, 0);
    NovmathPushVector3(L, to, 1);
    lua_pushvalue(L, groups_index);
    lua_pushvalue(L, groups_index + 1);
    NovmathCall(L, ENGINE_PHYSICS_RAYCAST, base, 1);
    NovmathPushPlain(L, base);
    return 1;
}

// physics.raycast_r(from_x, from_y, from_z, to_x, to_y, to_z, groups, [options])
static int PHYSICS_RaycastR(lua_State *L) { return Raycast(L, CheckV3R(L, 1), CheckV3R(L, 4), 7); }
// physics.raycast_t(from, to, groups, [options])
static int PHYSICS_RaycastT(lua_State *L) { return Raycast(L, CheckV3T(L, 1), CheckV3T(L, 2), 3); }

// physics.get_shape_t(url, shape)
static int PHYSICS_GetShapeT(lua_State *L)
{
    lua_settop(L, 2);
    NovmathCall(L, ENGINE_PHYSICS_GET_SHAPE, 1, 1);
    NovmathPushPlain(L, 1);
    return 1;
}

// Camera

// camera.get_view_t(camera, t_out) / camera.get_projection_t(camera, t_out)
static int CameraGetMatrixT(lua_State *L, NovmathEngineFn fn)
{
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_settop(L, 2);
    lua_pushvalue(L, 1);
    return NovmathCallUnpack(L, fn, 3, 2);
}

// Calls fn(position, [camera]) and returns the result as numbers or into the table at t_index
static int CameraConvert(lua_State *L, NovmathEngineFn fn, const dmVMath::Vector3 &v, int camera_index, int t_index)
{
    if (t_index)
        luaL_checktype(L, t_index, LUA_TTABLE);
    lua_settop(L, camera_index);
    int base = camera_index + 1;
    NovmathPushVector3(L, v, 0);
    lua_pushvalue(L, camera_index);
    return NovmathCallUnpack(L, fn, base, t_index);
}

static int CAMERA_GetViewT(lua_State *L) { return CameraGetMatrixT(L, ENGINE_CAMERA_GET_VIEW); }
static int CAMERA_GetProjectionT(lua_State *L) { return CameraGetMatrixT(L, ENGINE_CAMERA_GET_PROJECTION); }
// camera.screen_to_world_r(x, y, z, [camera]) -> x, y, z
static int CAMERA_ScreenToWorldR(lua_State *L) { return CameraConvert(L, ENGINE_CAMERA_SCREEN_TO_WORLD, CheckV3R(L, 1), 4, 0); }
// camera.screen_to_world_t(t_in, t_out, [camera])
static int CAMERA_ScreenToWorldT(lua_State *L) { return CameraConvert(L, ENGINE_CAMERA_SCREEN_TO_WORLD, CheckV3T(L, 1), 3, 2); }
static int CAMERA_WorldToScreenR(lua_State *L) { return CameraConvert(L, ENGINE_CAMERA_WORLD_TO_SCREEN, CheckV3R(L, 1), 4, 0); }
static int CAMERA_WorldToScreenT(lua_State *L) { return CameraConvert(L, ENGINE_CAMERA_WORLD_TO_SCREEN, CheckV3T(L, 1), 3, 2); }

// Render. Matrices are tables of 16 numbers in column-major order.

static int RenderSetMatrixT(lua_State *L, NovmathEngineFn fn)
{
    dmVMath::Matrix4 m = CheckM4T(L, 1);
    lua_settop(L, 0);
    NovmathPushMatrix4(L, m, 0);
    NovmathCall(L, fn, 1, 0);
    return 0;
}

static int RENDER_SetViewT(lua_State *L) { return RenderSetMatrixT(L, ENGINE_RENDER_SET_VIEW); }
static int RENDER_SetProjectionT(lua_State *L) { return RenderSetMatrixT(L, ENGINE_RENDER_SET_PROJECTION); }

// Replaces the table at index with a copy where vector/matrix tables are vmath values.
// Other values are copied as they are.
static void ConvertTableValues(lua_State *L, int index)
{
    int slots[KIND_MATRIX4 + 1] = {0};
    lua_newtable(L);
    int copy = lua_gettop(L);
    lua_pushnil(L);
    while (lua_next(L, index) != 0)
    {
        int value = lua_gettop(L);
        lua_pushvalue(L, value - 1);
        NovmathValue v;
        if (lua_istable(L, value) && NovmathReadTable(L, value, false, &v) != KIND_OTHER)
        {
            luaL_argcheck(L, slots[v.m_Kind] < NOVMATH_SCRATCH_SLOTS, index, "too many vector values");
            NovmathPushValue(L, v, slots[v.m_Kind]++);
        }
        else
        {
            lua_pushvalue(L, value);
        }
        lua_rawset(L, copy);
        lua_pop(L, 1);
    }
    lua_replace(L, index);
}

// render.clear_t(buffers): like render.clear, but colors are {x, y, z, w}
static int RENDER_ClearT(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1);
    ConvertTableValues(L, 1);
    NovmathCall(L, ENGINE_RENDER_CLEAR, 1, 0);
    return 0;
}

// render.draw_t(predicate, [options]): like render.draw, but `frustum` is 16 numbers
static int RENDER_DrawT(lua_State *L)
{
    lua_settop(L, 2);
    if (lua_istable(L, 2))
        ConvertTableValues(L, 2);
    NovmathCall(L, ENGINE_RENDER_DRAW, 1, 0);
    return 0;
}

// Replaces the table at index with a vector4 ({x, y, z, [w]}) or matrix4 (16 numbers)
static void ConvertConstant(lua_State *L, int index)
{
    NovmathValue v;
    luaL_argcheck(L, NovmathReadTable(L, index, false, &v) != KIND_OTHER, index, "expected {x, y, z, w} or 16 numbers");
    if (v.m_Kind != KIND_MATRIX4)
        v.m_Kind = KIND_VECTOR4;
    NovmathPushValue(L, v, 0);
    lua_replace(L, index);
}

// render.set_constant_r(constants, name, x, y, z, w)
static int RENDER_SetConstantR(lua_State *L)
{
    dmVMath::Vector4 v = CheckV4R(L, 3);
    luaL_checkstring(L, 2);
    lua_settop(L, 2);
    NovmathPushVector4(L, v, 0);
    lua_settable(L, 1);
    return 0;
}

// render.set_constant_t(constants, name, value)
// value is {x, y, z, w}, 16 numbers (matrix4), or an array of these
static int RENDER_SetConstantT(lua_State *L)
{
    luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);
    lua_settop(L, 3);

    lua_rawgeti(L, 3, 1);
    bool is_array = lua_istable(L, -1);
    lua_pop(L, 1);
    if (!is_array)
    {
        ConvertConstant(L, 3);
        lua_settable(L, 1);
        return 0;
    }

    // Assigning a table creates the array entry, which is then filled one element at a time
    lua_pushvalue(L, 2);
    lua_newtable(L);
    lua_settable(L, 1);
    lua_pushvalue(L, 2);
    lua_gettable(L, 1);
    int array = lua_gettop(L);
    int count = (int)lua_objlen(L, 3);
    for (int i = 1; i <= count; ++i)
    {
        lua_pushinteger(L, i);
        lua_rawgeti(L, 3, i);
        ConvertConstant(L, lua_gettop(L));
        lua_settable(L, array);
    }
    return 0;
}

const luaL_reg NovmathFactoryFunctions[] = {
    {"create_r", FACTORY_CreateR},
    {"create_t", FACTORY_CreateT},
    {0, 0}};

const luaL_reg NovmathSpriteFunctions[] = {
    {"set_constant_r", SPRITE_SetConstantR},
    {"set_constant_t", SPRITE_SetConstantT},
    {0, 0}};

const luaL_reg NovmathPhysicsFunctions[] = {
    {"raycast_r", PHYSICS_RaycastR},
    {"raycast_t", PHYSICS_RaycastT},
    {"get_shape_t", PHYSICS_GetShapeT},
    {0, 0}};

const luaL_reg NovmathCameraFunctions[] = {
    {"get_view_t", CAMERA_GetViewT},
    {"get_projection_t", CAMERA_GetProjectionT},
    {"screen_to_world_r", CAMERA_ScreenToWorldR},
    {"screen_to_world_t", CAMERA_ScreenToWorldT},
    {"world_to_screen_r", CAMERA_WorldToScreenR},
    {"world_to_screen_t", CAMERA_WorldToScreenT},
    {0, 0}};

const luaL_reg NovmathRenderFunctions[] = {
    {"set_view_t", RENDER_SetViewT},
    {"set_projection_t", RENDER_SetProjectionT},
    {"clear_t", RENDER_ClearT},
    {"draw_t", RENDER_DrawT},
    {"set_constant_r", RENDER_SetConstantR},
    {"set_constant_t", RENDER_SetConstantT},
    {0, 0}};
