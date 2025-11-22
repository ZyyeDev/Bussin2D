#pragma once

#include <lua.hpp>

#include "modules/keyboard/keyboard.h"
#include "variables.h"

void init_keyboard(){
    if (!g_keyboard){
        g_keyboard = new Keyboard();
    }
}

// lua: buss.keyboard.isDown("key")
static int lua_keyboard_isDown(lua_State* L){
    init_keyboard();

    const char* keyName = luaL_checkstring(L, 1);
    SDL_Scancode scancode = Keyboard::getScancodeFromName(keyName);

    lua_pushboolean(L, g_keyboard->isDown(scancode));
    return 1;
}

// lua: buss.keyboard.isPressed("key")
static int lua_keyboard_isPressed(lua_State* L){
    init_keyboard();

    const char* keyName = luaL_checkstring(L, 1);
    SDL_Scancode scancode = Keyboard::getScancodeFromName(keyName);

    lua_pushboolean(L, g_keyboard->isPressed(scancode));
    return 1;
}

// lua: buss.keyboard.isReleased("key")
static int lua_keyboard_isReleased(lua_State* L){
    init_keyboard();

    const char* keyName = luaL_checkstring(L, 1);
    SDL_Scancode scancode = Keyboard::getScancodeFromName(keyName);

    lua_pushboolean(L, g_keyboard->isReleased(scancode));
    return 1;
}

void register_keyboard_bindings(lua_State* L){
    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_keyboard_isDown);
    lua_setfield(L, -2, "isDown");

    lua_pushcfunction(L, lua_keyboard_isPressed);
    lua_setfield(L, -2, "isPressed");

    lua_pushcfunction(L, lua_keyboard_isReleased);
    lua_setfield(L, -2, "isReleased");

    lua_setfield(L, -2, "keyboard");
    lua_setglobal(L, "buss");
}