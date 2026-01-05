#pragma once

#include <lua.hpp>

#define PLUGIN_API_VERSION 1

// every plugin must have these funcs
extern "C"{
    // plugin loaded
    typedef int (*PluginInitFunc)(lua_State* L);
    // plugin unloaded
    typedef void (*PluginUnloadFunc)();
    // plugin info
    typedef const char* (*PluginGetNameFunc)();
    typedef int (*PluginGetVersionFunc)();
}

#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

/*
Use these in your plugins:
 PLUGIN_EXPORT int plugin_init(lua_State* L) { ... }
 PLUGIN_EXPORT void plugin_unload() { ... }
 PLUGIN_EXPORT const char* plugin_get_name() { return "MyPlugin"; }
 PLUGIN_EXPORT int plugin_get_version() { return 1; }
*/