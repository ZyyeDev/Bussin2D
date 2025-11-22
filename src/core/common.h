#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#define LUA_CHECK_ERROR(L, result) \
    if (result != LUA_OK) { \
        const char* error = lua_tostring(L,-1); \
        std::cerr << "Lua error: " << error << std::endl; \
        lua_pop(L, 1); \
        return false; \
    }