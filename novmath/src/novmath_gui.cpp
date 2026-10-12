#include <dmsdk/sdk.h>
#include <dmsdk/gui/gui.h>
#include "novmath_common.h"

// Property values are stored as Vector4 on the node. Setters only touch the
// components that are passed, matching gui.set_* which keeps `w` when given a vector3.

typedef void (*GuiSetFn)(lua_State *L, dmGui::HScene scene, dmGui::HNode node, dmGui::Property property, const dmVMath::Vector4 &value);

static void GuiSetNative(lua_State *L, dmGui::HScene scene, dmGui::HNode node, dmGui::Property property, const dmVMath::Vector4 &value)
{
    // gui.set_* ignores bones
    if (dmGui::GetNodeIsBone(scene, node))
        return;
    // Also keeps PROPERTY_ROTATION and PROPERTY_EULER in sync
    dmGui::SetNodeProperty(scene, node, property, value);
}

static void GuiSetSize(lua_State *L, dmGui::HScene scene, dmGui::HNode node, dmGui::Property property, const dmVMath::Vector4 &value)
{
    // The size mode isn't exposed in dmSDK, so let gui.set_size reject auto-sized nodes
    int base = lua_gettop(L) + 1;
    dmGui::LuaPushNode(L, scene, node);
    NovmathPushVector4(L, value, 0);
    NovmathCall(L, ENGINE_GUI_SET_SIZE, base, 0);
}

static inline dmVMath::Vector4
GuiGet(lua_State *L, dmGui::Property property)
{
    dmGui::HScene scene = dmGui::LuaCheckScene(L);
    dmGui::HNode node = dmGui::LuaCheckNode(L, 1);
    return dmGui::GetNodeProperty(scene, node, property);
}

static int GuiGetR(lua_State *L, dmGui::Property property, bool has_w)
{
    dmVMath::Vector4 v = GuiGet(L, property);
    return has_w ? PushV4R(L, v) : PushV3R(L, v.getXYZ());
}

static int GuiGetT(lua_State *L, dmGui::Property property, bool has_w)
{
    dmVMath::Vector4 v = GuiGet(L, property);
    luaL_checktype(L, 2, LUA_TTABLE);
    return has_w ? PushV4T(L, 2, v) : PushV3T(L, 2, v.getXYZ());
}

static int GuiGetN(lua_State *L, dmGui::Property property, int n)
{
    lua_pushnumber(L, GuiGet(L, property).getElem(n));
    return 1;
}

// x, y, z, [w]
static int GuiSetR(lua_State *L, dmGui::Property property, bool has_w, GuiSetFn set_fn)
{
    dmGui::HScene scene = dmGui::LuaCheckScene(L);
    dmGui::HNode node = dmGui::LuaCheckNode(L, 1);
    dmVMath::Vector4 v = dmGui::GetNodeProperty(scene, node, property);
    v.setXYZ(CheckV3R(L, 2));
    if (has_w && !lua_isnoneornil(L, 5))
        v.setW(luaL_checknumber(L, 5));
    set_fn(L, scene, node, property, v);
    return 0;
}

// {x, y, z, [w]}
static int GuiSetT(lua_State *L, dmGui::Property property, bool has_w, GuiSetFn set_fn)
{
    dmGui::HScene scene = dmGui::LuaCheckScene(L);
    dmGui::HNode node = dmGui::LuaCheckNode(L, 1);
    dmVMath::Vector4 v = dmGui::GetNodeProperty(scene, node, property);
    v = has_w ? CheckV4T(L, 2, v.getW()) : dmVMath::Vector4(CheckV3T(L, 2), v.getW());
    set_fn(L, scene, node, property, v);
    return 0;
}

static int GuiSetN(lua_State *L, dmGui::Property property, int n, GuiSetFn set_fn)
{
    dmGui::HScene scene = dmGui::LuaCheckScene(L);
    dmGui::HNode node = dmGui::LuaCheckNode(L, 1);
    dmVMath::Vector4 v = dmGui::GetNodeProperty(scene, node, property);
    v.setElem(n, luaL_checknumber(L, 2));
    set_fn(L, scene, node, property, v);
    return 0;
}

// X(lua_name, Name, property, set_fn)
#define GUI_V3_PROPS(X)                                    \
    X(position, Position, PROPERTY_POSITION, GuiSetNative) \
    X(scale, Scale, PROPERTY_SCALE, GuiSetNative)          \
    X(size, Size, PROPERTY_SIZE, GuiSetSize)               \
    X(euler, Euler, PROPERTY_EULER, GuiSetNative)

// Also have _w accessors
#define GUI_V4_PROPS(X)                                  \
    X(color, Color, PROPERTY_COLOR, GuiSetNative)        \
    X(outline, Outline, PROPERTY_OUTLINE, GuiSetNative)  \
    X(shadow, Shadow, PROPERTY_SHADOW, GuiSetNative)

#define GUI_DEFINE_N(Name, property, N, n, set_fn)                                           \
    static int GUI_Get##Name##N(lua_State *L) { return GuiGetN(L, dmGui::property, n); }     \
    static int GUI_Set##Name##N(lua_State *L) { return GuiSetN(L, dmGui::property, n, set_fn); }

#define GUI_DEFINE(Name, property, has_w, set_fn)                                                    \
    static int GUI_Get##Name##R(lua_State *L) { return GuiGetR(L, dmGui::property, has_w); }         \
    static int GUI_Set##Name##R(lua_State *L) { return GuiSetR(L, dmGui::property, has_w, set_fn); } \
    static int GUI_Get##Name##T(lua_State *L) { return GuiGetT(L, dmGui::property, has_w); }         \
    static int GUI_Set##Name##T(lua_State *L) { return GuiSetT(L, dmGui::property, has_w, set_fn); } \
    GUI_DEFINE_N(Name, property, X, 0, set_fn)                                                       \
    GUI_DEFINE_N(Name, property, Y, 1, set_fn)                                                       \
    GUI_DEFINE_N(Name, property, Z, 2, set_fn)

#define GUI_DEFINE_V3(lua_name, Name, property, set_fn) GUI_DEFINE(Name, property, false, set_fn)
#define GUI_DEFINE_V4(lua_name, Name, property, set_fn) \
    GUI_DEFINE(Name, property, true, set_fn)            \
    GUI_DEFINE_N(Name, property, W, 3, set_fn)

GUI_V3_PROPS(GUI_DEFINE_V3)
GUI_V4_PROPS(GUI_DEFINE_V4)

#define GUI_REGISTER_N(lua_name, Name, n, N)                    \
    {"get_" #lua_name "_" #n, GUI_Get##Name##N}, \
    {"set_" #lua_name "_" #n, GUI_Set##Name##N},

#define GUI_REGISTER_V3(lua_name, Name, property, set_fn) \
    GUI_REGISTER_N(lua_name, Name, r, R)                  \
    GUI_REGISTER_N(lua_name, Name, t, T)                  \
    GUI_REGISTER_N(lua_name, Name, x, X)                  \
    GUI_REGISTER_N(lua_name, Name, y, Y)                  \
    GUI_REGISTER_N(lua_name, Name, z, Z)

#define GUI_REGISTER_V4(lua_name, Name, property, set_fn) \
    GUI_REGISTER_V3(lua_name, Name, property, set_fn)     \
    GUI_REGISTER_N(lua_name, Name, w, W)

// Rotation (quaternion)

static int GUI_GetRotationR(lua_State *L)
{
    return PushV4R(L, GuiGet(L, dmGui::PROPERTY_ROTATION));
}

static int GUI_GetRotationT(lua_State *L)
{
    return GuiGetT(L, dmGui::PROPERTY_ROTATION, true);
}

static int GuiSetRotation(lua_State *L, const dmVMath::Vector4 &q)
{
    GuiSetNative(L, dmGui::LuaCheckScene(L), dmGui::LuaCheckNode(L, 1), dmGui::PROPERTY_ROTATION, q);
    return 0;
}

static int GUI_SetRotationR(lua_State *L) { return GuiSetRotation(L, CheckV4R(L, 2)); }
static int GUI_SetRotationT(lua_State *L) { return GuiSetRotation(L, CheckV4TStrict(L, 2)); }

// Screen position. Not exposed in dmSDK, so these forward to the engine.

// Calls fn(node, [position]) and returns the result as numbers or into the table at t_index
static int GuiCallUnpack(lua_State *L, NovmathEngineFn fn, const dmVMath::Vector3 *position, int t_index)
{
    if (t_index)
        luaL_checktype(L, t_index, LUA_TTABLE);
    lua_settop(L, t_index ? t_index : 1);
    int base = lua_gettop(L) + 1;
    lua_pushvalue(L, 1);
    if (position)
        NovmathPushVector3(L, *position, 0);
    return NovmathCallUnpack(L, fn, base, t_index);
}

static int GuiSetScreenPosition(lua_State *L, const dmVMath::Vector3 &position)
{
    lua_settop(L, 1);
    NovmathPushVector3(L, position, 0);
    NovmathCall(L, ENGINE_GUI_SET_SCREEN_POSITION, 1, 0);
    return 0;
}

static int GUI_GetScreenPositionR(lua_State *L) { return GuiCallUnpack(L, ENGINE_GUI_GET_SCREEN_POSITION, 0, 0); }
static int GUI_GetScreenPositionT(lua_State *L) { return GuiCallUnpack(L, ENGINE_GUI_GET_SCREEN_POSITION, 0, 2); }
static int GUI_SetScreenPositionR(lua_State *L) { return GuiSetScreenPosition(L, CheckV3R(L, 2)); }
static int GUI_SetScreenPositionT(lua_State *L) { return GuiSetScreenPosition(L, CheckV3T(L, 2)); }

// gui.screen_to_local_r(node, x, y, z) -> x, y, z
static int GUI_ScreenToLocalR(lua_State *L)
{
    dmVMath::Vector3 v = CheckV3R(L, 2);
    return GuiCallUnpack(L, ENGINE_GUI_SCREEN_TO_LOCAL, &v, 0);
}

// gui.screen_to_local_t(node, t_in, t_out)
static int GUI_ScreenToLocalT(lua_State *L)
{
    dmVMath::Vector3 v = CheckV3T(L, 2);
    return GuiCallUnpack(L, ENGINE_GUI_SCREEN_TO_LOCAL, &v, 3);
}

// Node creation. Forwarded to the engine, which does more than dmGui::NewNode
// (default properties, node limits, etc.).

// Calls fn(position, size) or fn(position, stack[arg_index])
static int GuiNewNode(lua_State *L, NovmathEngineFn fn, const dmVMath::Vector3 &position, const dmVMath::Vector3 *size, int arg_index)
{
    lua_settop(L, arg_index);
    NovmathPushVector3(L, position, 0);
    if (size)
        NovmathPushVector3(L, *size, 1);
    else
        lua_pushvalue(L, arg_index);
    NovmathCall(L, fn, arg_index + 1, 1);
    return 1;
}

static int GuiNewSizedNodeR(lua_State *L, NovmathEngineFn fn)
{
    dmVMath::Vector3 size = CheckV3R(L, 4);
    return GuiNewNode(L, fn, CheckV3R(L, 1), &size, 4);
}

static int GuiNewSizedNodeT(lua_State *L, NovmathEngineFn fn)
{
    dmVMath::Vector3 size = CheckV3T(L, 2);
    return GuiNewNode(L, fn, CheckV3T(L, 1), &size, 2);
}

static int GUI_NewBoxNodeR(lua_State *L) { return GuiNewSizedNodeR(L, ENGINE_GUI_NEW_BOX_NODE); }
static int GUI_NewBoxNodeT(lua_State *L) { return GuiNewSizedNodeT(L, ENGINE_GUI_NEW_BOX_NODE); }
static int GUI_NewPieNodeR(lua_State *L) { return GuiNewSizedNodeR(L, ENGINE_GUI_NEW_PIE_NODE); }
static int GUI_NewPieNodeT(lua_State *L) { return GuiNewSizedNodeT(L, ENGINE_GUI_NEW_PIE_NODE); }
static int GUI_NewTextNodeR(lua_State *L) { return GuiNewNode(L, ENGINE_GUI_NEW_TEXT_NODE, CheckV3R(L, 1), 0, 4); }
static int GUI_NewTextNodeT(lua_State *L) { return GuiNewNode(L, ENGINE_GUI_NEW_TEXT_NODE, CheckV3T(L, 1), 0, 2); }
static int GUI_NewParticlefxNodeR(lua_State *L) { return GuiNewNode(L, ENGINE_GUI_NEW_PARTICLEFX_NODE, CheckV3R(L, 1), 0, 4); }
static int GUI_NewParticlefxNodeT(lua_State *L) { return GuiNewNode(L, ENGINE_GUI_NEW_PARTICLEFX_NODE, CheckV3T(L, 1), 0, 2); }

// Generic properties (shader constants etc.), forwarded to the engine

static int GUI_SetR(lua_State *L) { return NovmathForwardSetR(L, ENGINE_GUI_SET); }
static int GUI_SetT(lua_State *L) { return NovmathForwardSetT(L, ENGINE_GUI_SET); }
static int GUI_GetR(lua_State *L) { return NovmathForwardGetR(L, ENGINE_GUI_GET); }
static int GUI_GetT(lua_State *L) { return NovmathForwardGetT(L, ENGINE_GUI_GET); }
// gui.animate_t(node, property, to, easing, duration, [delay], [complete_function], [playback])
static int GUI_AnimateT(lua_State *L) { return NovmathForwardAnimateT(L, ENGINE_GUI_ANIMATE, 3); }

const luaL_reg NovmathGuiFunctions[] = {
    GUI_V3_PROPS(GUI_REGISTER_V3)
    GUI_V4_PROPS(GUI_REGISTER_V4)
    {"get_rotation_r", GUI_GetRotationR},
    {"set_rotation_r", GUI_SetRotationR},
    {"get_rotation_t", GUI_GetRotationT},
    {"set_rotation_t", GUI_SetRotationT},
    {"get_screen_position_r", GUI_GetScreenPositionR},
    {"get_screen_position_t", GUI_GetScreenPositionT},
    {"set_screen_position_r", GUI_SetScreenPositionR},
    {"set_screen_position_t", GUI_SetScreenPositionT},
    {"screen_to_local_r", GUI_ScreenToLocalR},
    {"screen_to_local_t", GUI_ScreenToLocalT},
    {"new_box_node_r", GUI_NewBoxNodeR},
    {"new_box_node_t", GUI_NewBoxNodeT},
    {"new_pie_node_r", GUI_NewPieNodeR},
    {"new_pie_node_t", GUI_NewPieNodeT},
    {"new_text_node_r", GUI_NewTextNodeR},
    {"new_text_node_t", GUI_NewTextNodeT},
    {"new_particlefx_node_r", GUI_NewParticlefxNodeR},
    {"new_particlefx_node_t", GUI_NewParticlefxNodeT},
    {"set_r", GUI_SetR},
    {"set_t", GUI_SetT},
    {"get_r", GUI_GetR},
    {"get_t", GUI_GetT},
    {"animate_t", GUI_AnimateT},
    {0, 0}};
