#include <iostream>
#include <vector>

#include "plugin.h"
#include "plugin_interface.h"

PluginManager& PluginManager::get(){
    static PluginManager instance;
    return instance;
}

LibHandle PluginManager::loadLibrary(const std::string& path){
    #ifdef _WIN32
        return LoadLibraryA(path.c_str());
    #else
        return dlopen(path.c_str(), RTLD_LAZY);
    #endif
}

void* PluginManager::getFunction(LibHandle handle, const char* name){
    #ifdef _WIN32
        return (void*)GetProcAddress(handle, name);
    #else
        return dlsym(handle, name);
    #endif
}

void PluginManager::closeLibrary(LibHandle handle){
    #ifdef _WIN32
        FreeLibrary(handle);
    #else
        dlclose(handle);
    #endif
}

bool PluginManager::loadPlugin(const std::string& path, lua_State* L){
    std::cout << "Loading plugin " << path << std::endl;

    LibHandle handle = loadLibrary(path);
    if (!handle){
        #ifdef _WIN32
            std::cerr << "Failed to load plugin: " << GetLastError() << std::endl;
        #else
            std::cerr << "Failed to load plugin: " << dlerror() << std::endl;
        #endif
        return false;
    }

    auto getName = (PluginGetNameFunc)getFunction(handle, "plugin_get_name");
    auto getVersion = (PluginGetVersionFunc)getFunction(handle, "plugin_get_version");
    auto init = (PluginInitFunc)getFunction(handle, "plugin_init");

    if (!getName || !getVersion || !init){
        std::cerr << "Plugin is missing functions!" << std::endl;
        closeLibrary(handle);
        return false;
    }

    const char* name = getName();
    int version = getVersion();

    if (plugins.find(name) != plugins.end()){
        std::cerr << "plugin " << name << " already loaded!" << std::endl;
        closeLibrary(handle);
        return false;
    }

    if (init(L) != 0){
        std::cerr << "Plugin '"<< name << "' initialization failed" << std::endl;
        closeLibrary(handle);
        return false;
    }

    Plugin plugin;
    plugin.name = name;
    plugin.handle = handle;
    plugin.version = version;
    plugins[name] = plugin;

    std::cout << "Plugin " << name << " loaded!" << std::endl;
    return true;
}

void PluginManager::unloadPlugin(const std::string& name){
    auto it = plugins.find(name);
    if (it == plugins.end()){
        std::cerr << "Plugin was not loaded" << std::endl;
        return;
    }

    auto unload = (PluginUnloadFunc)getFunction(it->second.handle, "plugin_unload");
    if (unload){
        unload();
    }

    closeLibrary(it->second.handle);
    plugins.erase(it);

    std::cout << "Plugin " << name << " unloaded!" << std::endl;
}

void PluginManager::unloadAll(){
    for (auto& [name, plugin] : plugins){
        unloadPlugin(name);
    }
    plugins.clear();
}

bool PluginManager::isLoaded(const std::string& name){
    return plugins.find(name) != plugins.end();
}

std::vector<std::string> PluginManager::getLoadedPlugins(){
    std::vector<std::string> result;
    for (auto& [name, _] : plugins){
        result.push_back(name);
    }
    return result;
}