#pragma once
#include <lua.hpp>

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

void register_all_bindings(lua_State* L);
void register_window_bindings(lua_State* L);
void register_graphics_bindings(lua_State* L);
void register_audio_bindings(lua_State* L);
void register_keyboard_bindings(lua_State* L);
void register_mouse_bindings(lua_State* L);
void register_collision_bindings(lua_State* L);
