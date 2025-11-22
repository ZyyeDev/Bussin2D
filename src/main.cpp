#include <iostream>
#include <lua.hpp>
#include "bindings.h"
#include "core/common.h"

int main(int argc, char* argv[]){
    std::cout << "we bussin" << std::endl;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    register_all_bindings(L);

    int result = luaL_dofile(L, "lua/main.lua");
    if (result != LUA_OK){
        const char* error = lua_tostring(L, -1);
        std::cerr << "Lua Error: " << error << std::endl;
        lua_pop(L, 1);
        lua_close(L);
        return 1;
    }

    lua_close(L);

    return 0;
}