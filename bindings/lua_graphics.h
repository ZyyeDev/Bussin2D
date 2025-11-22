#pragma once

#include "modules/graphics/renderer.h"
#include "modules/window/window.h"
#include "core/common.h"
#include "variables.h"

#include <lua.hpp>

void init_renderer(){
    if (g_window && !g_renderer){
        g_renderer = new Renderer(g_window->getRenderer());
    }
}

// Lua: buss.graphics.drawRect(x, y, w, h, r, g, b, a)
static int lua_graphics_drawRect(lua_State* L){
    if (!g_renderer) init_renderer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    int g = luaL_checkinteger(L, 6);
    int b = luaL_checkinteger(L, 7);
    int a = luaL_checkinteger(L, 8);
    
    if (g_renderer){
        g_renderer->drawRect(x, y, w, h, r, g, b, a);
    }

    return 0;
}

// for now this is the same as drawRect cuz idk
// Lua: buss.graphics.drawRectFilled(x, y, w, h, r, g, b, a)
static int lua_graphics_drawRectFilled(lua_State* L){
    if (!g_renderer) init_renderer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    int g = luaL_checkinteger(L, 6);
    int b = luaL_checkinteger(L, 7);
    int a = luaL_checkinteger(L, 8);
    
    if (g_renderer){
        g_renderer->drawRect(x, y, w, h, r, g, b, a);
    }

    return 0;
}

// Lua: buss.graphics.drawCircle(x, y, radius, r, g, b, a)
static int lua_graphics_drawCircle(lua_State* L){
    if (!g_renderer) init_renderer();

    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int radius = luaL_checkinteger(L, 3);
    int r = luaL_optinteger(L, 4, 255);
    int g = luaL_optinteger(L, 5, 255);
    int b = luaL_optinteger(L, 6, 255);
    int a = luaL_optinteger(L, 7, 255);
    
    if (g_renderer){
        g_renderer->drawCircle(x, y, radius, r, g, b, a);
    }

    return 0;
}

// Lua: buss.graphics.drawLine(x1, y1, x2, y2, r, g, b, a)
static int lua_graphics_drawLine(lua_State* L) {
    if (!g_renderer) init_renderer();
    
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    int r = luaL_optinteger(L, 5, 255);
    int g = luaL_optinteger(L, 6, 255);
    int b = luaL_optinteger(L, 7, 255);
    int a = luaL_optinteger(L, 8, 255);
    
    if (g_renderer) {
        g_renderer->drawLine(x1, y1, x2, y2, r, g, b, a);
    }
    return 0;
}

// Lua: buss.graphics.loadTexture("path/to/image.png")
static int lua_graphics_loadTexture(lua_State* L) {
    if (!g_renderer) init_renderer();
    
    const char* path = luaL_checkstring(L, 1);
    
    if (g_renderer) {
        int id = g_renderer->loadTexture(path);
        lua_pushinteger(L, id);
        return 1;
    }
    
    lua_pushinteger(L, -1);
    return 1;
}

// Lua: buss.graphics.draw(textureId, x, y, width, height)
static int lua_graphics_draw(lua_State* L) {
    if (!g_renderer) init_renderer();
    
    int textureId = luaL_checkinteger(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int w = luaL_optinteger(L, 4, -1);
    int h = luaL_optinteger(L, 5, -1);
    
    if (g_renderer) {
        g_renderer->drawTexture(textureId, x, y, w, h);
    }
    return 0;
}

void register_graphics_bindings(lua_State* L){
    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_graphics_drawRect);
    lua_setfield(L, -2, "drawRect");

    lua_pushcfunction(L, lua_graphics_drawRectFilled);
    lua_setfield(L, -2, "drawRectFilled");

    lua_pushcfunction(L, lua_graphics_drawCircle);
    lua_setfield(L, -2, "drawCircle");
    
    lua_pushcfunction(L, lua_graphics_drawLine);
    lua_setfield(L, -2, "drawLine");
    
    lua_pushcfunction(L, lua_graphics_loadTexture);
    lua_setfield(L, -2, "loadTexture");
    
    lua_pushcfunction(L, lua_graphics_draw);
    lua_setfield(L, -2, "draw");

    lua_setfield(L, -2, "graphics");
    lua_setglobal(L, "buss");
}