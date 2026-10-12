#include <dmsdk/sdk.h>
#include "novmath.h"
#include "novmath_common.h"

static const struct
{
    const char *m_Name;
    const luaL_reg *m_Functions;
} MODULES[] = {
    {"go", NovmathGoFunctions},
    {"gui", NovmathGuiFunctions},
    {"factory", NovmathFactoryFunctions},
    {"sprite", NovmathSpriteFunctions},
    {"physics", NovmathPhysicsFunctions},
    {"camera", NovmathCameraFunctions},
    {"render", NovmathRenderFunctions},
    {"novmath", NovmathFunctions},
};

dmExtension::Result INITIALIZE(dmExtension::Params *params)
{
    lua_State *L = params->m_L;
    NovmathInit(L);

    // Adds to the engine's modules. Before Defold 1.14, gui, camera and render
    // functions are only usable from gui/render scripts when script.shared_state is enabled.
    for (size_t i = 0; i < sizeof(MODULES) / sizeof(MODULES[0]); ++i)
    {
        luaL_register(L, MODULES[i].m_Name, MODULES[i].m_Functions);
        lua_pop(L, 1);
    }
    dmLogInfo("Registered '%s' Extension", EXTENSION_NAME_STRING);
    return dmExtension::RESULT_OK;
}

dmExtension::Result FINALIZE(dmExtension::Params *params)
{
    NovmathFinalize(params->m_L);
    return dmExtension::RESULT_OK;
}

DECLARE_DEFOLD_EXTENSION
