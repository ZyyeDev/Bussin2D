#pragma once

#include <lua.h>
#include "modules/camera/camera.h"
#include "variables.h"

// Lua: buss.camera.setPosition(x, y)
static int lua_camera_setPosition(lua_State* L){
    init_camera();
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    if (g_camera) g_camera->setPosition(x, y);
    return 0;
}

// Lua: buss.camera.move(dx, dy)
static int lua_camera_move(lua_State* L){
    init_camera();
    float dx = luaL_checknumber(L, 1);
    float dy = luaL_checknumber(L, 2);
    if (g_camera) g_camera->move(dx, dy);
    return 0;
}

// Lua: buss.camera.setZoom(zoom)
static int lua_camera_setZoom(lua_State* L){
    init_camera();
    float zoom = luaL_checknumber(L, 1);
    if (g_camera) g_camera->setZoom(zoom);
    return 0;
}

// Lua: buss.camera.getX()
static int lua_camera_getX(lua_State* L){
    init_camera();
    if (g_camera){
        lua_pushnumber(L, g_camera->getX());
        return 1;
    }
    return 0;
}

// Lua: buss.camera.getY()
static int lua_camera_getY(lua_State* L){
    init_camera();
    if (g_camera){
        lua_pushnumber(L, g_camera->getY());
        return 1;
    }
    return 0;
}

// Lua: buss.camera.getZoom()
static int lua_camera_getZoom(lua_State* L){
    init_camera();
    if (g_camera){
        lua_pushnumber(L, g_camera->getZoom());
        return 1;
    }
    return 0;
}

void register_camera_bindings(lua_State* L){
    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }
    lua_newtable(L);

    lua_pushcfunction(L, lua_camera_setPosition);
    lua_setfield(L, -2, "setPosition");

    lua_pushcfunction(L, lua_camera_move);
    lua_setfield(L, -2, "move");

    lua_pushcfunction(L, lua_camera_setZoom);
    lua_setfield(L, -2, "setZoom");

    lua_pushcfunction(L, lua_camera_getX);
    lua_setfield(L, -2, "getX");

    lua_pushcfunction(L, lua_camera_getY);
    lua_setfield(L, -2, "getY");

    lua_pushcfunction(L, lua_camera_getZoom);
    lua_setfield(L, -2, "getZoom");

    lua_setfield(L, -2, "camera");
    lua_setglobal(L, "buss");
}