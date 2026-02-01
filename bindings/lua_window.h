#pragma once

#include <lua.hpp>
#include "modules/window/window.h"
#include "core/common.h"
#include "variables.h"

// lua: buss.window.init(width, height, title)
static int lua_window_init(lua_State* L){
    int width = luaL_checkinteger(L,1);
    int height = luaL_checkinteger(L,2);
    const char* title = luaL_checkstring(L,3);

    if (!g_window){
        g_window = new Window();
    }

    bool success = g_window->init(width, height, title);
    lua_pushboolean(L,success);
    return 1;
}

// lua: buss.window.setVSync(enabled)
static int lua_window_setVSync(lua_State* L) {
    bool enabled;

    if (lua_isboolean(L, 1)) {
        enabled = lua_toboolean(L, 1);
    } else {
        enabled = luaL_checkinteger(L, 1) != 0;
    }

    if (g_window) {
        g_window->setVsync(enabled);
    }
    return 0;
}

// lua: buss.window.setMaxFPS(fps)
static int lua_window_setMaxFPS(lua_State* L){
    int targetFps = luaL_checkinteger(L,1);

    if (g_window){
        g_window->setTargetFPS(targetFps);
    }
    return 0;
}

// lua: buss.window.getMaxFPS()
static int lua_window_getMaxFPS(lua_State* L){
    if (g_window){
        lua_pushnumber(L, g_window->getTargetFPS());
    }
    return 1;
}

// lua: buss.window.getFps()
static int lua_window_getFps(lua_State* L){
    if (g_window){
        lua_pushnumber(L, g_window->currentFPS);
    }
    return 1;
}

// lua: buss.window.isRunning()
static int lua_window_isRunning(lua_State* L){
    if (!g_window){
        lua_pushboolean(L, false);
        return 1;
    }

    lua_pushboolean(L,g_window->isRunning);
    return 1;
}

// lua: buss.window.clear()
static int lua_window_clear(lua_State* L){
    int r = luaL_checkinteger(L, 1);
    int g = luaL_checkinteger(L, 2);
    int b = luaL_checkinteger(L, 3);

    if (g_window){
        g_window->clear(r,g,b);
    }
    return 0;
}

// lua: buss.window.present()
static int lua_window_present(lua_State* L){
    if (g_window){
        g_window->present();
    }
    return 0;
}

// lua: buss.window.getWidth()
static int lua_window_getWidth(lua_State* L){
    if (g_window){
        lua_pushnumber(L, g_window->getWidth());
    }
    return 1;
}

// lua: buss.window.getHeight()
static int lua_window_getHeight(lua_State* L){
    if (g_window){
        lua_pushnumber(L, g_window->getHeight());
    }
    return 1;
}

// lua: buss.window.setPhysicsFramerate(frame_rate)
static int lua_set_physics_framerate(lua_State* L){
    if (g_window){
        g_window->set_physics_framerate(luaL_checkinteger(L, 0));
    }
    return 1;
}

void register_window_bindings(lua_State* L){
    lua_getglobal(L,"buss");
    if (lua_isnil(L, -1)){
        lua_pop(L,1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_window_init);
    lua_setfield(L, -2, "init");

    lua_pushcfunction(L, lua_window_isRunning);
    lua_setfield(L, -2, "isRunning");

    lua_pushcfunction(L, lua_window_clear);
    lua_setfield(L, -2, "clear");

    lua_pushcfunction(L, lua_window_present);
    lua_setfield(L, -2, "present");

    lua_pushcfunction(L, lua_window_getWidth);
    lua_setfield(L, -2, "getWidth");

    lua_pushcfunction(L, lua_window_getHeight);
    lua_setfield(L, -2, "getHeight");
    
    lua_pushcfunction(L, lua_window_setVSync);
    lua_setfield(L, -2, "setVSync");
    
    lua_pushcfunction(L, lua_window_setMaxFPS);
    lua_setfield(L, -2, "setMaxFPS");
    
    lua_pushcfunction(L, lua_window_getMaxFPS);
    lua_setfield(L, -2, "getMaxFPS");
    
    lua_pushcfunction(L, lua_window_getFps);
    lua_setfield(L, -2, "getFps");

    lua_pushcfunction(L, lua_set_physics_framerate);
    lua_setfield(L, -2, "setPhysicsFramerate");

    lua_setfield(L, -2, "window");
    lua_setglobal(L, "buss");
}