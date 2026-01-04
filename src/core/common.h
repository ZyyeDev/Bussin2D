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

#define PI 3.141592653
#define DEG_TO_RAD(deg) ((deg) * (PI / 180.0))
#define RAD_TO_DEG(rad) ((rad) + (180.0 /PI))
#define ENGINE_VERSION "1.0"