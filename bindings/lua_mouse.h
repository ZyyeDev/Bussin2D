#pragma once

#include <lua.hpp>
#include "modules/mouse/mouse.h"
#include "variables.h"

void init_mouse(){
    if (!g_mouse){
        g_mouse = new Mouse();
    }
}

// lua: buss.mouse.getX()
static int lua_mouse_getX(lua_State* L){
    init_mouse();
    lua_pushinteger(L, g_mouse->getX());
    return 1;
}

// lua: buss.mouse.getY()
static int lua_mouse_getY(lua_State* L){
    init_mouse();
    lua_pushinteger(L, g_mouse->getY());
    return 1;
}

// lua: buss.mouse.getPosition()
static int lua_mouse_getPosition(lua_State* L){
    init_mouse();
    lua_pushinteger(L, g_mouse->getX());
    lua_pushinteger(L, g_mouse->getY());
    return 2;
}

// lua: buss.mouse.isDown(mouse button)
static int lua_mouse_isDown(lua_State* L){
    init_mouse();
    int button = luaL_checkinteger(L, 1);
    lua_pushboolean(L, g_mouse->isDown(button));
    return 1;
}

// lua: buss.mouse.isPressed(mouse button)
static int lua_mouse_isPressed(lua_State* L){
    init_mouse();
    int button = luaL_checkinteger(L, 1);
    lua_pushboolean(L, g_mouse->isPressed(button));
    return 1;
}

// lua: buss.mouse.isReleased(mouse button)
static int lua_mouse_isReleased(lua_State* L){
    init_mouse();
    int button = luaL_checkinteger(L, 1);
    lua_pushboolean(L, g_mouse->isReleased(button));
    return 1;
}

// lua: buss.mouse.isReleased(mouse button)
static int lua_mouse_getScroll(lua_State* L){
    init_mouse();
    lua_pushnumber(L, g_mouse->getScrollY());
    return 1;
}

void register_mouse_bindings(lua_State* L){
    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_mouse_getX);
    lua_setfield(L, -2, "getX");

    lua_pushcfunction(L, lua_mouse_getY);
    lua_setfield(L, -2, "getY");

    lua_pushcfunction(L, lua_mouse_getPosition);
    lua_setfield(L, -2, "getPosition");

    lua_pushcfunction(L, lua_mouse_isDown);
    lua_setfield(L, -2, "isDown");

    lua_pushcfunction(L, lua_mouse_isPressed);
    lua_setfield(L, -2, "isPressed");

    lua_pushcfunction(L, lua_mouse_isReleased);
    lua_setfield(L, -2, "isReleased");

    lua_pushcfunction(L, lua_mouse_getScroll);
    lua_setfield(L, -2, "getScroll");

    lua_setfield(L, -2, "mouse");
    lua_setglobal(L, "buss");
}