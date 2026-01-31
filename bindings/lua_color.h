#pragma once

#include <lua.hpp>
#include "core/common.h"
#include "core/color.h"
#include "variables.h"

// lua: Color.rgbtohsv(h,s,v)
static int lua_color_rgb_to_hsv(lua_State* L){
    float r = luaL_checknumber(L, 1);
    float g = luaL_checknumber(L, 2);
    float b = luaL_checknumber(L, 3);

    auto oHSV = RGBtoHSV(r,g,b);

    lua_pushnumber(L, oHSV.H);
    lua_pushnumber(L, oHSV.S);
    lua_pushnumber(L, oHSV.V);

    return 3;
}

// lua: Color.hsvtorgb(r,g,b)
static int lua_color_hsb_to_rgb(lua_State* L){
    float h = luaL_checknumber(L, 1);
    float s = luaL_checknumber(L, 2);
    float v = luaL_checknumber(L, 3);

    auto oRGB = HSVtoRGB(h,s,v);

    lua_pushnumber(L, oRGB.R);
    lua_pushnumber(L, oRGB.G);
    lua_pushnumber(L, oRGB.B);

    return 3;
}

void register_color_bindings(lua_State* L){
    lua_getglobal(L,"buss");
    if (lua_isnil(L, -1)){
        lua_pop(L,1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_color_rgb_to_hsv);
    lua_setfield(L, -2, "rgbtohsv");

    lua_pushcfunction(L, lua_color_hsb_to_rgb);
    lua_setfield(L, -2, "hsvtorgb");

    lua_setglobal(L, "Color");
}