#include <string.h>
#include <dmsdk/sdk.h>
#include "novmath_common.h"

struct EngineFnName
{
    const char *m_Module;
    const char *m_Name;
};

static const EngineFnName ENGINE_FUNCTIONS[ENGINE_COUNT] = {
#define NOVMATH_NAME(id, module, name) {module, name},
    NOVMATH_ENGINE_FUNCTIONS(NOVMATH_NAME)
#undef NOVMATH_NAME
};

// Registry references, resolved on first use: engine modules may register after the extension
static int g_EngineRefs[ENGINE_COUNT];

// Reusable vmath values, indexed by NovmathKind. Lua never moves userdata, so the
// value pointers stay valid while the registry holds the references.
static int g_ScratchRefs[KIND_MATRIX4 + 1][NOVMATH_SCRATCH_SLOTS];
static void *g_Scratch[KIND_MATRIX4 + 1][NOVMATH_SCRATCH_SLOTS];

static const dmhash_t HASH_ROTATION = dmHashString64("rotation");

void NovmathInit(lua_State *L)
{
    for (int i = 0; i < ENGINE_COUNT; ++i)
    {
        g_EngineRefs[i] = LUA_NOREF;
    }

    NovmathValue value;
    value.m_V = dmVMath::Vector4(0, 0, 0, 1);
    value.m_M = dmVMath::Matrix4::identity();
    for (int kind = KIND_VECTOR3; kind <= KIND_MATRIX4; ++kind)
    {
        value.m_Kind = (NovmathKind)kind;
        for (int slot = 0; slot < NOVMATH_SCRATCH_SLOTS; ++slot)
        {
            NovmathPushValue(L, value, -1);
            switch (kind)
            {
            case KIND_VECTOR3: g_Scratch[kind][slot] = dmScript::ToVector3(L, -1); break;
            case KIND_VECTOR4: g_Scratch[kind][slot] = dmScript::ToVector4(L, -1); break;
            case KIND_QUAT: g_Scratch[kind][slot] = dmScript::ToQuat(L, -1); break;
            default: g_Scratch[kind][slot] = dmScript::ToMatrix4(L, -1); break;
            }
            g_ScratchRefs[kind][slot] = luaL_ref(L, LUA_REGISTRYINDEX);
        }
    }
}

void NovmathFinalize(lua_State *L)
{
    for (int i = 0; i < ENGINE_COUNT; ++i)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, g_EngineRefs[i]);
        g_EngineRefs[i] = LUA_NOREF;
    }
    for (int kind = KIND_VECTOR3; kind <= KIND_MATRIX4; ++kind)
    {
        for (int slot = 0; slot < NOVMATH_SCRATCH_SLOTS; ++slot)
        {
            luaL_unref(L, LUA_REGISTRYINDEX, g_ScratchRefs[kind][slot]);
            g_ScratchRefs[kind][slot] = LUA_NOREF;
            g_Scratch[kind][slot] = 0;
        }
    }
}

static void PushEngineFunction(lua_State *L, NovmathEngineFn fn)
{
    if (g_EngineRefs[fn] != LUA_NOREF)
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, g_EngineRefs[fn]);
        return;
    }
    const EngineFnName &name = ENGINE_FUNCTIONS[fn];
    lua_getglobal(L, name.m_Module);
    if (!lua_istable(L, -1))
    {
        luaL_error(L, "novmath: module '%s' is not available in this context", name.m_Module);
    }
    lua_getfield(L, -1, name.m_Name);
    if (!lua_isfunction(L, -1))
    {
        luaL_error(L, "novmath: function '%s.%s' is not available in this context", name.m_Module, name.m_Name);
    }
    lua_remove(L, -2);
    lua_pushvalue(L, -1);
    g_EngineRefs[fn] = luaL_ref(L, LUA_REGISTRYINDEX);
}

void NovmathCall(lua_State *L, NovmathEngineFn fn, int base, int nresults)
{
    int top = lua_gettop(L);
    while (top >= base && lua_isnil(L, top))
    {
        --top;
    }
    lua_settop(L, top);
    PushEngineFunction(L, fn);
    lua_insert(L, base);
    lua_call(L, top - base + 1, nresults);
}

int NovmathCallUnpack(lua_State *L, NovmathEngineFn fn, int base, int t_index)
{
    NovmathCall(L, fn, base, 1);
    return t_index ? NovmathUnpackT(L, base, t_index) : NovmathUnpackR(L, base);
}

// Table <-> vmath

NovmathKind NovmathReadTable(lua_State *L, int index, bool is_quat, NovmathValue *out)
{
    if (index < 0)
        index = lua_gettop(L) + index + 1;
    out->m_Kind = KIND_OTHER;

    lua_getfield(L, index, "x");
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 1);
        for (int i = 0; i < 16; ++i)
        {
            lua_rawgeti(L, index, i + 1);
            bool ok = lua_isnumber(L, -1);
            float value = (float)lua_tonumber(L, -1);
            lua_pop(L, 1);
            if (!ok)
                return KIND_OTHER;
            out->m_M.setElem(i / 4, i % 4, value);
        }
        return out->m_Kind = KIND_MATRIX4;
    }

    lua_getfield(L, index, "y");
    lua_getfield(L, index, "z");
    lua_getfield(L, index, "w");
    bool has_w = !lua_isnil(L, -1);
    bool ok = lua_isnumber(L, -4) && lua_isnumber(L, -3) && lua_isnumber(L, -2) && (!has_w || lua_isnumber(L, -1));
    out->m_V = dmVMath::Vector4(lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
    lua_pop(L, 4);
    if (!ok)
        return KIND_OTHER;

    if (!has_w)
        return out->m_Kind = KIND_VECTOR3;
    if (!is_quat)
    {
        lua_getfield(L, index, "quat");
        is_quat = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    return out->m_Kind = is_quat ? KIND_QUAT : KIND_VECTOR4;
}

void NovmathPushValue(lua_State *L, const NovmathValue &value, int slot)
{
    if (slot < 0)
    {
        switch (value.m_Kind)
        {
        case KIND_VECTOR3: dmScript::PushVector3(L, value.m_V.getXYZ()); break;
        case KIND_VECTOR4: dmScript::PushVector4(L, value.m_V); break;
        case KIND_QUAT: dmScript::PushQuat(L, dmVMath::Quat(value.m_V)); break;
        case KIND_MATRIX4: dmScript::PushMatrix4(L, value.m_M); break;
        default: lua_pushnil(L); break;
        }
        return;
    }

    void *scratch = g_Scratch[value.m_Kind][slot];
    lua_rawgeti(L, LUA_REGISTRYINDEX, g_ScratchRefs[value.m_Kind][slot]);
    switch (value.m_Kind)
    {
    case KIND_VECTOR3: *(dmVMath::Vector3 *)scratch = value.m_V.getXYZ(); break;
    case KIND_VECTOR4: *(dmVMath::Vector4 *)scratch = value.m_V; break;
    case KIND_QUAT: *(dmVMath::Quat *)scratch = dmVMath::Quat(value.m_V); break;
    case KIND_MATRIX4: *(dmVMath::Matrix4 *)scratch = value.m_M; break;
    default: break;
    }
}

void NovmathConvertArg(lua_State *L, int index, int slot, bool is_quat)
{
    if (!lua_istable(L, index))
        return;
    NovmathValue value;
    luaL_argcheck(L, NovmathReadTable(L, index, is_quat, &value) != KIND_OTHER, index,
                  "expected {x, y, z, [w]} or 16 numbers");
    NovmathPushValue(L, value, slot);
    lua_replace(L, index);
}

bool NovmathIsQuatProperty(lua_State *L, int index)
{
    if (lua_type(L, index) == LUA_TSTRING)
    {
        size_t len;
        const char *name = lua_tolstring(L, index, &len);
        return len == 8 && memcmp(name, "rotation", 8) == 0;
    }
    return dmScript::CheckHashOrString(L, index) == HASH_ROTATION;
}

// vmath -> Lua

static NovmathKind ReadVmath(lua_State *L, int index, NovmathValue *out)
{
    out->m_Kind = KIND_OTHER;
    if (lua_type(L, index) != LUA_TUSERDATA)
        return KIND_OTHER;
    if (dmVMath::Vector3 *v3 = dmScript::ToVector3(L, index))
    {
        out->m_V = dmVMath::Vector4(*v3, 0);
        return out->m_Kind = KIND_VECTOR3;
    }
    if (dmVMath::Vector4 *v4 = dmScript::ToVector4(L, index))
    {
        out->m_V = *v4;
        return out->m_Kind = KIND_VECTOR4;
    }
    if (dmVMath::Quat *q = dmScript::ToQuat(L, index))
    {
        out->m_V = dmVMath::Vector4(*q);
        return out->m_Kind = KIND_QUAT;
    }
    if (dmVMath::Matrix4 *m = dmScript::ToMatrix4(L, index))
    {
        out->m_M = *m;
        return out->m_Kind = KIND_MATRIX4;
    }
    return KIND_OTHER;
}

static int PushValueR(lua_State *L, const NovmathValue &value)
{
    switch (value.m_Kind)
    {
    case KIND_VECTOR3: return PushV3R(L, value.m_V.getXYZ());
    case KIND_MATRIX4:
        luaL_checkstack(L, 16, "novmath: cannot return matrix");
        for (int i = 0; i < 16; ++i)
            lua_pushnumber(L, value.m_M.getElem(i / 4, i % 4));
        return 16;
    default: return PushV4R(L, value.m_V);
    }
}

static void PushValueT(lua_State *L, int t_index, const NovmathValue &value)
{
    luaL_checktype(L, t_index, LUA_TTABLE);
    switch (value.m_Kind)
    {
    case KIND_VECTOR3: PushV3T(L, t_index, value.m_V.getXYZ()); break;
    case KIND_MATRIX4:
        for (int i = 0; i < 16; ++i)
        {
            lua_pushnumber(L, value.m_M.getElem(i / 4, i % 4));
            lua_rawseti(L, t_index, i + 1);
        }
        break;
    default: PushV4T(L, t_index, value.m_V); break;
    }
}

// Pushes a plain table for a vmath value. Returns false, pushing nothing, for other values.
static bool PushPlainVmath(lua_State *L, int index)
{
    NovmathValue value;
    if (ReadVmath(L, index, &value) == KIND_OTHER)
        return false;
    if (value.m_Kind == KIND_MATRIX4)
        lua_createtable(L, 16, 0);
    else
        lua_createtable(L, 0, 4);
    PushValueT(L, lua_gettop(L), value);
    return true;
}

static void ConvertTableInPlace(lua_State *L, int index, int depth)
{
    if (depth > 16)
        return; // guard against cycles
    lua_pushnil(L);
    while (lua_next(L, index) != 0)
    {
        int value = lua_gettop(L);
        if (PushPlainVmath(L, value))
        {
            // Assigning to an existing key during traversal is allowed
            lua_pushvalue(L, value - 1);
            lua_insert(L, -2);
            lua_rawset(L, index);
        }
        else if (lua_istable(L, value))
        {
            ConvertTableInPlace(L, value, depth + 1);
        }
        lua_pop(L, 1);
    }
}

void NovmathPushPlain(lua_State *L, int index)
{
    if (index < 0)
        index = lua_gettop(L) + index + 1;
    if (PushPlainVmath(L, index))
        return;
    if (lua_istable(L, index))
        ConvertTableInPlace(L, index, 0);
    lua_pushvalue(L, index);
}

int NovmathUnpackR(lua_State *L, int index)
{
    NovmathValue value;
    if (ReadVmath(L, index, &value) != KIND_OTHER)
        return PushValueR(L, value);
    NovmathPushPlain(L, index);
    return 1;
}

int NovmathUnpackT(lua_State *L, int index, int t_index)
{
    NovmathValue value;
    if (ReadVmath(L, index, &value) != KIND_OTHER)
    {
        PushValueT(L, t_index, value);
        return 0;
    }
    NovmathPushPlain(L, index);
    return 1;
}

// Generic property forwarding

// (url, property, x, [y, z, [w]], [options])
int NovmathForwardSetR(lua_State *L, NovmathEngineFn fn)
{
    int count = 0;
    while (lua_type(L, 3 + count) == LUA_TNUMBER)
        ++count;

    NovmathValue value;
    switch (count)
    {
    case 1:
        break;
    case 3:
        value.m_Kind = KIND_VECTOR3;
        value.m_V = dmVMath::Vector4(CheckV3R(L, 3), 0);
        break;
    case 4:
        value.m_Kind = NovmathIsQuatProperty(L, 2) ? KIND_QUAT : KIND_VECTOR4;
        value.m_V = CheckV4R(L, 3);
        break;
    default:
        return luaL_error(L, "novmath: expected 1, 3 or 4 numbers, got %d", count);
    }
    if (count > 1)
    {
        NovmathPushValue(L, value, 0);
        lua_replace(L, 3);
        for (int i = 1; i < count; ++i)
            lua_remove(L, 4);
    }
    NovmathCall(L, fn, 1, 0);
    return 0;
}

// (url, property, value, [options])
int NovmathForwardSetT(lua_State *L, NovmathEngineFn fn)
{
    NovmathConvertArg(L, 3, 0, lua_istable(L, 3) && NovmathIsQuatProperty(L, 2));
    NovmathCall(L, fn, 1, 0);
    return 0;
}

// (url, property, [options])
int NovmathForwardGetR(lua_State *L, NovmathEngineFn fn)
{
    return NovmathCallUnpack(L, fn, 1, 0);
}

// (url, property, t_out, [options])
int NovmathForwardGetT(lua_State *L, NovmathEngineFn fn)
{
    luaL_checktype(L, 3, LUA_TTABLE);
    // Move t_out below the forwarded arguments
    lua_pushvalue(L, 3);
    lua_insert(L, 1);
    lua_remove(L, 4);
    return NovmathCallUnpack(L, fn, 2, 1);
}

int NovmathForwardAnimateT(lua_State *L, NovmathEngineFn fn, int value_index)
{
    NovmathConvertArg(L, value_index, 0, lua_istable(L, value_index) && NovmathIsQuatProperty(L, 2));
    NovmathCall(L, fn, 1, 0);
    return 0;
}

// novmath.plain(value): replaces vmath values with plain tables, converting tables in place.
// Use it on engine messages in on_message.
static int Novmath_Plain(lua_State *L)
{
    luaL_checkany(L, 1);
    NovmathPushPlain(L, 1);
    return 1;
}

const luaL_reg NovmathFunctions[] = {
    {"plain", Novmath_Plain},
    {0, 0}};
