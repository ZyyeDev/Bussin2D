#pragma once
#include <lua.hpp>

#include "modules/plugin/plugin.h"

// Lua: buss.plugins.load("path")
static int lua_plugin_load(lua_State* L){
    const char* path = luaL_checkstring(L, 1);

    bool success = PluginManager::get().loadPlugin(path, L);
    lua_pushboolean(L, success);
    return 1;
}

// Lua: buss.plugins.unload("PluginName")
static int lua_plugin_unload(lua_State* L){
    const char* name = luaL_checkstring(L, 1);
    PluginManager::get().unloadPlugin(name);
    return 0;
}

// Lua: buss.plugins.list()
static int lua_plugin_list(lua_State* L){
    auto plugins = PluginManager::get().getLoadedPlugins();

    lua_newtable(L);
    for (size_t i = 0; i < plugins.size(); i++){
        lua_pushstring(L, plugins[i].c_str());
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}

// Lua: buss.plugins.isLoaded("PluginName")
static int lua_plugin_isLoaded(lua_State* L){
    const char* name = luaL_checkstring(L, 1);
    lua_pushboolean(L, PluginManager::get().isLoaded(name));
    return 1;
}

void register_plugin_bindings(lua_State* L){
    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_plugin_load);
    lua_setfield(L, -2, "load");

    lua_pushcfunction(L, lua_plugin_unload);
    lua_setfield(L, -2, "unload");

    lua_pushcfunction(L, lua_plugin_list);
    lua_setfield(L, -2, "list");

    lua_pushcfunction(L, lua_plugin_isLoaded);
    lua_setfield(L, -2, "isLoaded");

    lua_setfield(L, -2, "plugins");
    lua_setglobal(L, "buss");
}