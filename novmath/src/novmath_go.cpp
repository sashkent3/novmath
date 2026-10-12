#include <dmsdk/sdk.h>
#include "novmath_common.h"

typedef dmGameObject::HInstance Instance;
typedef dmVMath::Vector3 (*GoGetFn)(Instance);
typedef void (*GoSetFn)(Instance, const dmVMath::Vector3 &);

static dmVMath::Vector3 QuatToEuler(const dmVMath::Quat &q)
{
    return dmVMath::QuatToEuler(q.getX(), q.getY(), q.getZ(), q.getW());
}

static dmVMath::Vector3 GetPosition(Instance i) { return dmVMath::Vector3(dmGameObject::GetPosition(i)); }
static void SetPosition(Instance i, const dmVMath::Vector3 &v) { dmGameObject::SetPosition(i, dmVMath::Point3(v)); }
static dmVMath::Vector3 GetScale(Instance i) { return dmGameObject::GetScale(i); }
static void SetScale(Instance i, const dmVMath::Vector3 &v) { dmGameObject::SetScale(i, v); }
static dmVMath::Vector3 GetEuler(Instance i) { return QuatToEuler(dmGameObject::GetRotation(i)); }
static void SetEuler(Instance i, const dmVMath::Vector3 &v) { dmGameObject::SetRotation(i, dmVMath::EulerToQuat(v)); }
static dmVMath::Vector3 GetWorldPosition(Instance i) { return dmVMath::Vector3(dmGameObject::GetWorldPosition(i)); }
static dmVMath::Vector3 GetWorldScale(Instance i) { return dmGameObject::GetWorldScale(i); }
static dmVMath::Vector3 GetWorldEuler(Instance i) { return QuatToEuler(dmGameObject::GetWorldRotation(i)); }

static int GoGetR(lua_State *L, GoGetFn get)
{
    return PushV3R(L, get(dmScript::CheckGOInstance(L, 1)));
}

static int GoGetT(lua_State *L, GoGetFn get)
{
    Instance instance = dmScript::CheckGOInstance(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    return PushV3T(L, 2, get(instance));
}

static int GoGetN(lua_State *L, GoGetFn get, int n)
{
    lua_pushnumber(L, get(dmScript::CheckGOInstance(L, 1)).getElem(n));
    return 1;
}

static int GoSetR(lua_State *L, GoSetFn set)
{
    set(dmScript::CheckGOInstance(L, 1), CheckV3R(L, 2));
    return 0;
}

static int GoSetT(lua_State *L, GoSetFn set)
{
    set(dmScript::CheckGOInstance(L, 1), CheckV3T(L, 2));
    return 0;
}

static int GoSetN(lua_State *L, GoGetFn get, GoSetFn set, int n)
{
    Instance instance = dmScript::CheckGOInstance(L, 1);
    dmVMath::Vector3 v = get(instance);
    v.setElem(n, luaL_checknumber(L, 2));
    set(instance, v);
    return 0;
}

// Readable properties: X(lua_name, Name)
#define GO_GET_PROPS(X)               \
    X(position, Position)             \
    X(scale, Scale)                   \
    X(euler, Euler)                   \
    X(world_position, WorldPosition)  \
    X(world_scale, WorldScale)        \
    X(world_euler, WorldEuler)

// Writable properties
#define GO_SET_PROPS(X) \
    X(position, Position) \
    X(scale, Scale)       \
    X(euler, Euler)

#define GO_DEFINE_GET(lua_name, Name)                                               \
    static int GO_Get##Name##R(lua_State *L) { return GoGetR(L, Get##Name); }       \
    static int GO_Get##Name##T(lua_State *L) { return GoGetT(L, Get##Name); }       \
    static int GO_Get##Name##X(lua_State *L) { return GoGetN(L, Get##Name, 0); }    \
    static int GO_Get##Name##Y(lua_State *L) { return GoGetN(L, Get##Name, 1); }    \
    static int GO_Get##Name##Z(lua_State *L) { return GoGetN(L, Get##Name, 2); }

#define GO_DEFINE_SET(lua_name, Name)                                                         \
    static int GO_Set##Name##R(lua_State *L) { return GoSetR(L, Set##Name); }                 \
    static int GO_Set##Name##T(lua_State *L) { return GoSetT(L, Set##Name); }                 \
    static int GO_Set##Name##X(lua_State *L) { return GoSetN(L, Get##Name, Set##Name, 0); }   \
    static int GO_Set##Name##Y(lua_State *L) { return GoSetN(L, Get##Name, Set##Name, 1); }   \
    static int GO_Set##Name##Z(lua_State *L) { return GoSetN(L, Get##Name, Set##Name, 2); }

#define GO_REGISTER(prefix, Prefix, lua_name, Name)      \
    {#prefix "_" #lua_name "_r", GO_##Prefix##Name##R}, \
    {#prefix "_" #lua_name "_t", GO_##Prefix##Name##T}, \
    {#prefix "_" #lua_name "_x", GO_##Prefix##Name##X}, \
    {#prefix "_" #lua_name "_y", GO_##Prefix##Name##Y}, \
    {#prefix "_" #lua_name "_z", GO_##Prefix##Name##Z},

#define GO_REGISTER_GET(lua_name, Name) GO_REGISTER(get, Get, lua_name, Name)
#define GO_REGISTER_SET(lua_name, Name) GO_REGISTER(set, Set, lua_name, Name)

GO_GET_PROPS(GO_DEFINE_GET)
GO_SET_PROPS(GO_DEFINE_SET)

// Rotation (quaternion)

static int GO_GetRotationR(lua_State *L)
{
    return PushV4R(L, dmVMath::Vector4(dmGameObject::GetRotation(dmScript::CheckGOInstance(L, 1))));
}

static int GO_SetRotationR(lua_State *L)
{
    dmGameObject::SetRotation(dmScript::CheckGOInstance(L, 1), dmVMath::Quat(CheckV4R(L, 2)));
    return 0;
}

static int GO_GetRotationT(lua_State *L)
{
    Instance instance = dmScript::CheckGOInstance(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    return PushV4T(L, 2, dmVMath::Vector4(dmGameObject::GetRotation(instance)));
}

static int GO_SetRotationT(lua_State *L)
{
    dmGameObject::SetRotation(dmScript::CheckGOInstance(L, 1), dmVMath::Quat(CheckV4TStrict(L, 2)));
    return 0;
}

static int GO_GetWorldRotationR(lua_State *L)
{
    return PushV4R(L, dmVMath::Vector4(dmGameObject::GetWorldRotation(dmScript::CheckGOInstance(L, 1))));
}

static int GO_GetWorldRotationT(lua_State *L)
{
    Instance instance = dmScript::CheckGOInstance(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    return PushV4T(L, 2, dmVMath::Vector4(dmGameObject::GetWorldRotation(instance)));
}

// Generic properties, forwarded to the engine

static int GO_SetR(lua_State *L) { return NovmathForwardSetR(L, ENGINE_GO_SET); }
static int GO_SetT(lua_State *L) { return NovmathForwardSetT(L, ENGINE_GO_SET); }
static int GO_GetR(lua_State *L) { return NovmathForwardGetR(L, ENGINE_GO_GET); }
static int GO_GetT(lua_State *L) { return NovmathForwardGetT(L, ENGINE_GO_GET); }
// go.animate_t(url, property, playback, to, easing, duration, [delay], [complete_function])
static int GO_AnimateT(lua_State *L) { return NovmathForwardAnimateT(L, ENGINE_GO_ANIMATE, 4); }

const luaL_reg NovmathGoFunctions[] = {
    GO_GET_PROPS(GO_REGISTER_GET)
    GO_SET_PROPS(GO_REGISTER_SET)
    {"get_rotation_r", GO_GetRotationR},
    {"set_rotation_r", GO_SetRotationR},
    {"get_rotation_t", GO_GetRotationT},
    {"set_rotation_t", GO_SetRotationT},
    {"get_world_rotation_r", GO_GetWorldRotationR},
    {"get_world_rotation_t", GO_GetWorldRotationT},
    {"set_r", GO_SetR},
    {"set_t", GO_SetT},
    {"get_r", GO_GetR},
    {"get_t", GO_GetT},
    {"animate_t", GO_AnimateT},
    {0, 0}};
