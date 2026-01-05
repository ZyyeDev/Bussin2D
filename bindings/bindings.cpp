#include <bindings.h>

#include "lua_window.h"
#include "lua_graphics.h"
#include "lua_audio.h"
#include "lua_keyboard.h"
#include "lua_mouse.h"
#include "lua_collision.h"
#include "lua_camera.h"
#include "lua_plugin.h"

void register_all_bindings(lua_State* L){
    lua_pushcfunction(L, lua_sleep);
    lua_setglobal(L, "wait");
    
    register_window_bindings(L);
    register_graphics_bindings(L);
    register_audio_bindings(L);
    register_keyboard_bindings(L);
    register_mouse_bindings(L);
    register_collision_bindings(L);
    register_camera_bindings(L);
    register_plugin_bindings(L);
};