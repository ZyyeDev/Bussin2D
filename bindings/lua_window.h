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

// lua: buss.window.clear(r, g, b)
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

// lua: buss.window.setPhysicsFramerate(fps)
static int lua_set_physics_framerate(lua_State* L){
    if (g_window){
        g_window->set_physics_framerate(luaL_checkinteger(L, 1));
    }
    return 0;
}

// lua: buss.window.setFullscreen(fullscreen)
static int lua_window_setFullscreen(lua_State* L){
    bool fullscreen = lua_toboolean(L, 1) != 0;
    if (g_window) g_window->setFullscreen(fullscreen);
    return 0;
}

// lua: buss.window.setBorderless(borderless)
static int lua_window_setBorderless(lua_State* L){
    bool borderless = lua_toboolean(L, 1) != 0;
    if (g_window) g_window->setBorderless(borderless);
    return 0;
}

// lua: buss.window.isFullscreen()
static int lua_window_isFullscreen(lua_State* L){
    if (g_window) lua_pushboolean(L, g_window->isFullscreen());
    return 1;
}

// lua: buss.window.isBorderless()
static int lua_window_isBorderless(lua_State* L){
    if (g_window) lua_pushboolean(L, g_window->isBorderless());
    return 1;
}

// lua: buss.window.setSize(width, height)
static int lua_window_setSize(lua_State* L){
    int w = luaL_checkinteger(L, 1);
    int h = luaL_checkinteger(L, 2);

    if (g_window) g_window->setSize(w, h);
    return 0;
}

// lua: buss.window.setTitle(title)
static int lua_window_setTitle(lua_State* L){
    const char* title = luaL_checkstring(L, 1);
    if (g_window) g_window->setTitle(title);
    return 0;
}

// lua: buss.window.getTitle()
static int lua_window_getTitle(lua_State* L){
    if (g_window){
        lua_pushstring(L, g_window->getTitle().c_str());
        return 1;
    }
    return 0;
}

// lua: buss.window.setPosition(x, y)
static int lua_window_setPosition(lua_State* L){
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    if (g_window) g_window->setPosition(x, y);
    return 0;
}

// lua: buss.window.getPosition()
static int lua_window_getPosition(lua_State* L){
    if (g_window){
        int x = 0, y = 0;
        g_window->getPosition(&x, &y);
        lua_pushinteger(L, x);
        lua_pushinteger(L, y);
        return 2;
    }
    return 0;
}

// lua: buss.window.maximize()
static int lua_window_maximize(lua_State* L){
    if (g_window) g_window->maximize();
    return 0;
}

// lua: buss.window.minimize()
static int lua_window_minimize(lua_State* L){
    if (g_window) g_window->minimize();
    return 0;
}

// lua: buss.window.restore()
static int lua_window_restore(lua_State* L){
    if (g_window) g_window->restore();
    return 0;
}

// lua: buss.window.hasFocus()
static int lua_window_hasFocus(lua_State* L){
    if (g_window) lua_pushboolean(L, g_window->hasFocus());
    return 1;
}

// lua: buss.window.isMaximized()
static int lua_window_isMaximized(lua_State* L){
    if (g_window) lua_pushboolean(L, g_window->isMaximized());
    return 1;
}

// lua: buss.window.isMinimized()
static int lua_window_isMinimized(lua_State* L){
    if (g_window) lua_pushboolean(L, g_window->isMinimized());
    return 1;
}

// lua: buss.window.setResizable(bool)
static int lua_window_setResizable(lua_State* L){
    bool r = lua_toboolean(L, 1) != 0;
    if (g_window) g_window->setResizable(r);
    return 0;
}

// lua: buss.window.isResizable()
static int lua_window_isResizable(lua_State* L){
    if (g_window) lua_pushboolean(L, g_window->isResizable());
    return 1;
}

// lua: buss.window.setVirtualResolution(width, height)
static int lua_window_setVirtualResolution(lua_State* L){
    int w = luaL_checkinteger(L, 1);
    int h = luaL_checkinteger(L, 2);
    if (g_renderer) g_renderer->setVirtualResolution(w, h);
    return 0;
}

// lua: buss.window.clearVirtualResolution()
static int lua_window_clearVirtualResolution(lua_State* L){
    if (g_renderer) g_renderer->clearVirtualResolution();
    return 0;
}

// lua: buss.window.getVirtualResolution()
static int lua_window_getVirtualResolution(lua_State* L){
    if (g_renderer && g_renderer->hasVirtualResolution()){
        lua_pushinteger(L, g_renderer->getVirtualWidth());
        lua_pushinteger(L, g_renderer->getVirtualHeight());
    } else {
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
    }
    return 2;
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

    lua_pushcfunction(L, lua_window_setFullscreen);
    lua_setfield(L, -2, "setFullscreen");

    lua_pushcfunction(L, lua_window_setBorderless);
    lua_setfield(L, -2, "setBorderless");

    lua_pushcfunction(L, lua_window_isFullscreen);
    lua_setfield(L, -2, "isFullscreen");

    lua_pushcfunction(L, lua_window_isBorderless);
    lua_setfield(L, -2, "isBorderless");

    lua_pushcfunction(L, lua_window_setSize);
    lua_setfield(L, -2, "setSize");

    lua_pushcfunction(L, lua_window_setTitle);
    lua_setfield(L, -2, "setTitle");

    lua_pushcfunction(L, lua_window_getTitle);
    lua_setfield(L, -2, "getTitle");

    lua_pushcfunction(L, lua_window_setPosition);
    lua_setfield(L, -2, "setPosition");

    lua_pushcfunction(L, lua_window_getPosition);
    lua_setfield(L, -2, "getPosition");

    lua_pushcfunction(L, lua_window_maximize);
    lua_setfield(L, -2, "maximize");

    lua_pushcfunction(L, lua_window_minimize);
    lua_setfield(L, -2, "minimize");

    lua_pushcfunction(L, lua_window_restore);
    lua_setfield(L, -2, "restore");

    lua_pushcfunction(L, lua_window_hasFocus);
    lua_setfield(L, -2, "hasFocus");

    lua_pushcfunction(L, lua_window_isMaximized);
    lua_setfield(L, -2, "isMaximized");

    lua_pushcfunction(L, lua_window_isMinimized);
    lua_setfield(L, -2, "isMinimized");

    lua_pushcfunction(L, lua_window_setResizable);
    lua_setfield(L, -2, "setResizable");

    lua_pushcfunction(L, lua_window_isResizable);
    lua_setfield(L, -2, "isResizable");

    lua_pushcfunction(L, lua_window_setVirtualResolution);
    lua_setfield(L, -2, "setVirtualResolution");

    lua_pushcfunction(L, lua_window_clearVirtualResolution);
    lua_setfield(L, -2, "clearVirtualResolution");

    lua_pushcfunction(L, lua_window_getVirtualResolution);
    lua_setfield(L, -2, "getVirtualResolution");

    lua_setfield(L, -2, "window");
    lua_setglobal(L, "buss");
}
