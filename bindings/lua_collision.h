#pragma once

#include <lua.h>
#include "modules/collision/collision.h"
#include "variables.h"

static int lua_collision_AABB(lua_State* L) {
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int w1 = luaL_checkinteger(L, 3);
    int h1 = luaL_checkinteger(L, 4);
    int x2 = luaL_checkinteger(L, 5);
    int y2 = luaL_checkinteger(L, 6);
    int w2 = luaL_checkinteger(L, 7);
    int h2 = luaL_checkinteger(L, 8);
    
    lua_pushboolean(L, Collision::AABB(x1, y1, w1, h1, x2, y2, w2, h2));
    return 1;
}

static int lua_collision_Circle(lua_State* L) {
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int r1 = luaL_checkinteger(L, 3);
    int x2 = luaL_checkinteger(L, 4);
    int y2 = luaL_checkinteger(L, 5);
    int r2 = luaL_checkinteger(L, 6);
    
    lua_pushboolean(L, Collision::Circle(x1, y1, r1, x2, y2, r2));
    return 1;
}

static int lua_collision_PointInRect(lua_State* L) {
    int px = luaL_checkinteger(L, 1);
    int py = luaL_checkinteger(L, 2);
    int rx = luaL_checkinteger(L, 3);
    int ry = luaL_checkinteger(L, 4);
    int rw = luaL_checkinteger(L, 5);
    int rh = luaL_checkinteger(L, 6);
    
    lua_pushboolean(L, Collision::PointInRect(px, py, rx, ry, rw, rh));
    return 1;
}

void register_collision_bindings(lua_State* L) {
    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }
    
    lua_newtable(L);
    
    lua_pushcfunction(L, lua_collision_AABB);
    lua_setfield(L, -2, "AABB");
    
    lua_pushcfunction(L, lua_collision_Circle);
    lua_setfield(L, -2, "circle");
    
    lua_pushcfunction(L, lua_collision_PointInRect);
    lua_setfield(L, -2, "pointInRect");
    
    lua_setfield(L, -2, "collision");
    lua_setglobal(L, "buss");
}