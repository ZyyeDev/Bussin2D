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
#define LERP(a, b, t) ((a) + (t) * ((b) - (a)))
#define ENGINE_VERSION "1.0"

namespace Colors{
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
}