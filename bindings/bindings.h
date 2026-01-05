#pragma once
#include <lua.hpp>
#include <iostream>

#include "modules/vfs/vfs.h"

#ifdef _WIN32
    #include <windows.h>
    #define sleep(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleep(ms) usleep(ms)
#endif

static int lua_sleep(lua_State *L){
    float m = static_cast<float> (luaL_checknumber(L,1));
    sleep(m * 1000);

    return 0;
}

static int vfs_lua_loader(lua_State *L){
    std::string name = luaL_checkstring(L, 1);
    std::string path = name + ".lua";

    std::string code = VFS::get().readText(path);
    if (code.empty()){
        lua_pushfstring(L, "\n\tno file '%s'", path.c_str());
        return 1;
    }
    if (luaL_loadbuffer(L, code.c_str(), code.size(), path.c_str()) != LUA_OK){
        return lua_error(L);
    }
    return 1;
}

static void install_vfs_loader(lua_State* L){
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "loaders");

    int n = lua_objlen(L, -1);
    lua_pushcfunction(L, vfs_lua_loader);
    lua_rawseti(L, -2, n+1);

    lua_pop(L, 2);
}

void register_all_bindings(lua_State* L);
void register_window_bindings(lua_State* L);
void register_graphics_bindings(lua_State* L);
void register_audio_bindings(lua_State* L);
void register_keyboard_bindings(lua_State* L);
void register_mouse_bindings(lua_State* L);
void register_collision_bindings(lua_State* L);
void register_camera_bindings(lua_State* L);
