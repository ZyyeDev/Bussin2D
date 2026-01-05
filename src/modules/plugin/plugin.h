#pragma once

#include <iostream>
#include <unordered_map>
#include "lua.hpp"

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibHandle;
#else
    #include <dlfcn.h>
    typedef void* LibHandle;
#endif

struct Plugin{
    std::string name;
    LibHandle handle;
    int version;
};

class PluginManager{
public:
    static PluginManager& get();

    bool loadPlugin(const std::string& path, lua_State* L);
    void unloadPlugin(const std::string& name);
    void unloadAll();

    bool isLoaded(const std::string& path);
    std::vector<std::string> getLoadedPlugins();
private:
    PluginManager() = default;
    std::unordered_map<std::string,Plugin> plugins;

    LibHandle loadLibrary(const std::string& path);
    void* getFunction(LibHandle handle, const char* name);
    void closeLibrary(LibHandle handle);
};