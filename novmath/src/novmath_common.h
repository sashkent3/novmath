#ifndef NOVMATH_COMMON_H
#define NOVMATH_COMMON_H

#include <dmsdk/sdk.h>

// Engine Lua functions the extension forwards to, for things dmSDK doesn't expose.
#define NOVMATH_ENGINE_FUNCTIONS(X)                          \
    X(GO_SET, "go", "set")                                   \
    X(GO_GET, "go", "get")                                   \
    X(GO_ANIMATE, "go", "animate")                           \
    X(GUI_SET, "gui", "set")                                 \
    X(GUI_GET, "gui", "get")                                 \
    X(GUI_ANIMATE, "gui", "animate")                         \
    X(GUI_SET_SIZE, "gui", "set_size")                       \
    X(GUI_GET_SCREEN_POSITION, "gui", "get_screen_position") \
    X(GUI_SET_SCREEN_POSITION, "gui", "set_screen_position") \
    X(GUI_SCREEN_TO_LOCAL, "gui", "screen_to_local")         \
    X(GUI_NEW_BOX_NODE, "gui", "new_box_node")               \
    X(GUI_NEW_PIE_NODE, "gui", "new_pie_node")               \
    X(GUI_NEW_TEXT_NODE, "gui", "new_text_node")             \
    X(GUI_NEW_PARTICLEFX_NODE, "gui", "new_particlefx_node") \
    X(FACTORY_CREATE, "factory", "create")                   \
    X(PHYSICS_RAYCAST, "physics", "raycast")                 \
    X(PHYSICS_GET_SHAPE, "physics", "get_shape")             \
    X(CAMERA_GET_VIEW, "camera", "get_view")                 \
    X(CAMERA_GET_PROJECTION, "camera", "get_projection")     \
    X(CAMERA_SCREEN_TO_WORLD, "camera", "screen_to_world")   \
    X(CAMERA_WORLD_TO_SCREEN, "camera", "world_to_screen")   \
    X(RENDER_SET_VIEW, "render", "set_view")                 \
    X(RENDER_SET_PROJECTION, "render", "set_projection")     \
    X(RENDER_CLEAR, "render", "clear")                       \
    X(RENDER_DRAW, "render", "draw")

enum NovmathEngineFn
{
#define NOVMATH_ENUM(id, module, name) ENGINE_##id,
    NOVMATH_ENGINE_FUNCTIONS(NOVMATH_ENUM)
#undef NOVMATH_ENUM
    ENGINE_COUNT
};

// Number of reusable vmath values per type. A single call never needs more than
// this many values of one type at once (render.clear_t needs one per color buffer).
#define NOVMATH_SCRATCH_SLOTS 4

enum NovmathKind
{
    KIND_OTHER,
    KIND_VECTOR3,
    KIND_VECTOR4,
    KIND_QUAT,
    KIND_MATRIX4,
};

struct NovmathValue
{
    NovmathKind m_Kind;
    dmVMath::Vector4 m_V; // vector3 (w unused), vector4 and quat
    dmVMath::Matrix4 m_M;
};

void NovmathInit(lua_State *L);
void NovmathFinalize(lua_State *L);

// Calls the engine function with stack[base..top] as arguments, leaving `nresults`
// results at base. Trailing nils are dropped, since some engine functions reject an
// explicit nil where they accept a missing argument.
void NovmathCall(lua_State *L, NovmathEngineFn fn, int base, int nresults);

// NovmathCall with one result, returned as numbers (t_index == 0) or written into
// the table at t_index (see NovmathUnpackR / NovmathUnpackT).
int NovmathCallUnpack(lua_State *L, NovmathEngineFn fn, int base, int t_index);

// Reads a table into `out`: {x, y, z} -> vector3, {x, y, z, w} -> vector4, or quat
// if is_quat or the table has `quat = true`, 16 numbers -> matrix4 (column-major).
// Returns KIND_OTHER for anything else.
NovmathKind NovmathReadTable(lua_State *L, int index, bool is_quat, NovmathValue *out);

// Pushes the value as vmath. slot >= 0 uses a reusable value, which the engine
// copies, so it's only valid until the next push to the same slot. slot < 0 allocates.
void NovmathPushValue(lua_State *L, const NovmathValue &value, int slot);

static inline void NovmathPushVector3(lua_State *L, const dmVMath::Vector3 &v, int slot)
{
    NovmathValue value;
    value.m_Kind = KIND_VECTOR3;
    value.m_V = dmVMath::Vector4(v, 0);
    NovmathPushValue(L, value, slot);
}

static inline void NovmathPushVector4(lua_State *L, const dmVMath::Vector4 &v, int slot)
{
    NovmathValue value;
    value.m_Kind = KIND_VECTOR4;
    value.m_V = v;
    NovmathPushValue(L, value, slot);
}

static inline void NovmathPushMatrix4(lua_State *L, const dmVMath::Matrix4 &m, int slot)
{
    NovmathValue value;
    value.m_Kind = KIND_MATRIX4;
    value.m_M = m;
    NovmathPushValue(L, value, slot);
}

// Replaces a table argument with the vmath value the engine expects (see
// NovmathReadTable). Other values are left as they are.
void NovmathConvertArg(lua_State *L, int index, int slot, bool is_quat);

// True if the property at index is "rotation", which takes a quat.
bool NovmathIsQuatProperty(lua_State *L, int index);

// Pushes the value at index with all vmath values replaced by plain tables:
// vector3 -> {x, y, z}, vector4/quat -> {x, y, z, w}, matrix4 -> 16 numbers.
// Tables are converted in place, recursively.
void NovmathPushPlain(lua_State *L, int index);

// Returns the vmath value at index as numbers (16 for a matrix4). Other values
// are returned as plain values (see NovmathPushPlain).
int NovmathUnpackR(lua_State *L, int index);

// Writes the vmath value at index into the table at t_index and returns nothing.
// Other values are returned as plain values instead.
int NovmathUnpackT(lua_State *L, int index, int t_index);

// go.set/gui.set, go.get/gui.get and go.animate/gui.animate with plain values.
int NovmathForwardSetR(lua_State *L, NovmathEngineFn fn);
int NovmathForwardSetT(lua_State *L, NovmathEngineFn fn);
int NovmathForwardGetR(lua_State *L, NovmathEngineFn fn);
int NovmathForwardGetT(lua_State *L, NovmathEngineFn fn);
int NovmathForwardAnimateT(lua_State *L, NovmathEngineFn fn, int value_index);

// Module function tables
extern const luaL_reg NovmathGoFunctions[];
extern const luaL_reg NovmathGuiFunctions[];
extern const luaL_reg NovmathFactoryFunctions[];
extern const luaL_reg NovmathSpriteFunctions[];
extern const luaL_reg NovmathPhysicsFunctions[];
extern const luaL_reg NovmathCameraFunctions[];
extern const luaL_reg NovmathRenderFunctions[];
extern const luaL_reg NovmathFunctions[];

static inline dmVMath::Vector3
CheckV3R(lua_State *L, int index)
{
    lua_Number x = luaL_checknumber(L, index);
    lua_Number y = luaL_checknumber(L, index + 1);
    lua_Number z = luaL_checknumber(L, index + 2);
    return dmVMath::Vector3(x, y, z);
}

static inline dmVMath::Vector4
CheckV4R(lua_State *L, int index)
{
    return dmVMath::Vector4(CheckV3R(L, index), luaL_checknumber(L, index + 3));
}

static inline int
PushV3R(lua_State *L, const dmVMath::Vector3 &v)
{
    lua_pushnumber(L, v.getX());
    lua_pushnumber(L, v.getY());
    lua_pushnumber(L, v.getZ());
    return 3;
}

static inline int
PushV4R(lua_State *L, const dmVMath::Vector4 &v)
{
    PushV3R(L, v.getXYZ());
    lua_pushnumber(L, v.getW());
    return 4;
}

static inline bool
ReadVectorT(lua_State *L, int index, NovmathValue *v)
{
    return lua_istable(L, index) && NovmathReadTable(L, index, false, v) != KIND_OTHER && v->m_Kind != KIND_MATRIX4;
}

// Reads {x, y, z}
static inline dmVMath::Vector3
CheckV3T(lua_State *L, int index)
{
    NovmathValue v;
    luaL_argcheck(L, ReadVectorT(L, index, &v), index, "expected {x, y, z}");
    return v.m_V.getXYZ();
}

// Reads {x, y, z, [w]}, using default_w when w is missing
static inline dmVMath::Vector4
CheckV4T(lua_State *L, int index, float default_w)
{
    NovmathValue v;
    luaL_argcheck(L, ReadVectorT(L, index, &v), index, "expected {x, y, z, [w]}");
    if (v.m_Kind == KIND_VECTOR3)
        v.m_V.setW(default_w);
    return v.m_V;
}

// Reads {x, y, z, w}, w required
static inline dmVMath::Vector4
CheckV4TStrict(lua_State *L, int index)
{
    NovmathValue v;
    luaL_argcheck(L, ReadVectorT(L, index, &v) && v.m_Kind != KIND_VECTOR3, index, "expected {x, y, z, w}");
    return v.m_V;
}

// Reads 16 numbers in column-major order (t[1..4] is the first column)
static inline dmVMath::Matrix4
CheckM4T(lua_State *L, int index)
{
    NovmathValue v;
    luaL_argcheck(L, lua_istable(L, index) && NovmathReadTable(L, index, false, &v) == KIND_MATRIX4,
                  index, "expected 16 numbers");
    return v.m_M;
}

static inline int
PushV3T(lua_State *L, int index, const dmVMath::Vector3 &v)
{
    lua_pushnumber(L, v.getX());
    lua_setfield(L, index, "x");
    lua_pushnumber(L, v.getY());
    lua_setfield(L, index, "y");
    lua_pushnumber(L, v.getZ());
    lua_setfield(L, index, "z");
    return 0;
}

static inline int
PushV4T(lua_State *L, int index, const dmVMath::Vector4 &v)
{
    PushV3T(L, index, v.getXYZ());
    lua_pushnumber(L, v.getW());
    lua_setfield(L, index, "w");
    return 0;
}

#endif // NOVMATH_COMMON_H
