#pragma once

#include <lua.hpp>
#include "core/common.h"
#include "variables.h"

// lua: buss.fs.writeText(path, content)
static int lua_set_writeText(lua_State* L){
    std::string path = luaL_checkstring(L, 0);
    std::string content = luaL_checkstring(L, 1);
    bool success = g_vfs->writeText(path, content);
    lua_pushboolean(L, success);
    return 1;
}

// lua: buss.fs.readText(path)
static int lua_set_readText(lua_State* L){
    std::string path = luaL_checkstring(L, 0);

    std::string content = g_vfs->readText(path);
    lua_pushstring(L, content.c_str());
    return 1;
}

// lua: buss.fs.writeBinary(path, content)
static int lua_set_writeBinary(lua_State* L)
{
    std::string path = luaL_checkstring(L, 1);

    size_t len;
    const char* data = luaL_checklstring(L, 2, &len);

    std::vector<unsigned char> buffer(
        reinterpret_cast<const unsigned char*>(data),
        reinterpret_cast<const unsigned char*>(data) + len
    );
    
    bool success = g_vfs->writeBinary(path, buffer);

    lua_pushboolean(L, success);
    return 1;
}

// lua: buss.fs.readBinary(path)
static int lua_set_readBinary(lua_State* L)
{
    std::string path = luaL_checkstring(L, 1);

    std::vector<unsigned char> data = g_vfs->readBinary(path);

    if (data.empty()) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(
        L,
        reinterpret_cast<const char*>(data.data()),
        data.size()
    );

    return 1;
}

void register_fs_bindings(lua_State* L){
    lua_getglobal(L,"buss");
    if (lua_isnil(L, -1)){
        lua_pop(L,1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_set_writeText);
    lua_setfield(L, -2, "writeText");
    
    lua_pushcfunction(L, lua_set_readText);
    lua_setfield(L, -2, "readText");
    
    lua_pushcfunction(L, lua_set_writeBinary);
    lua_setfield(L, -2, "writeBinary");
    
    lua_pushcfunction(L, lua_set_readBinary);
    lua_setfield(L, -2, "readBinary");

    lua_setfield(L, -2, "fs");
    lua_setglobal(L, "buss");
}