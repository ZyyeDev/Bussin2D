#pragma once

#include <lua.hpp>
#include "core/common.h"
#include "core/math.h"
#include "variables.h"

// lua: math.flerp(a,b,t)
static int lua_math_flerp(lua_State* L){
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    float lerpResult = flerp(a,b,t);

    lua_pushnumber(L, lerpResult);

    return 1;
}

void register_math_bindings(lua_State* L){
    lua_getglobal(L, "math");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }

    lua_pushcfunction(L, lua_math_flerp);
    lua_setfield(L, -2, "flerp");

    lua_pop(L, 1);
}
