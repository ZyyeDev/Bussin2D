#pragma once
#include <lua.hpp>

void register_all_bindings(lua_State* L);
void register_window_bindings(lua_State* L);
void register_graphics_bindings(lua_State* L);
void register_audio_bindings(lua_State* L);