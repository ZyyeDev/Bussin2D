#include <iostream>
#include <fstream>
#include <filesystem>
#include "core/common.h"

#ifdef _WIN32
    #include <windows.h>
    #include <libloaderapi.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

#include "vfs.h"

namespace fs = std::filesystem;

VFS& VFS::get(){
    static VFS instance;
    return instance;
}

std::string VFS::getExePath(){
    #ifdef _WIN32
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        return std::string(path);
    #else
        char path[PATH_MAX + 1];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        if (count <= 0) {
            return "";
        }
        path[count] = '\0';
        return std::string(path);
    #endif
}

struct Footer{
    uint64_t offset;
    uint64_t size;
    char magic[8];
};

bool VFS::loadPackageData(){
    exePath = getExePath();
    std::ifstream exe(exePath, std::ios::binary);
    if (!exe.is_open()){
        std::cerr << "exe is not open??" << std::endl;
        return false;
    }

    exe.seekg(-static_cast<int>(sizeof(Footer)), std::ios::end);
    Footer footer;
    exe.read((char*)&footer, sizeof(Footer));

    std::cout << "[VFS] Magic: " << std::string(footer.magic, 7) << std::endl;

    if (std::string(footer.magic, 6) != "BUSSIN"){
        std::cerr << "Invalid package!!" << std::endl;
        return false;
    }

    std::cout << "[VFS] Valid package found, offset: " << footer.offset << std::endl;

    exe.seekg(footer.offset);
    uint32_t count;
    exe.read((char*)&count, sizeof(count));

    std::cout << "[VFS] Loading " << count << " files" << std::endl;

    uint64_t currentOffset = exe.tellg();

    for (uint32_t i = 0; i < count; i++){
        uint32_t len;
        exe.read((char*)&len, sizeof(len));

        std::string name(len, '\0');
        exe.read(&name[0], len);

        uint32_t size;
        exe.read((char*)&size, sizeof(size));

        FileEntry entry;
        entry.offset = currentOffset + sizeof(len) + len + sizeof(size);
        entry.size = size;
        entry.loaded = false;

        files[name] = entry;

        exe.seekg(size, std::ios::cur);
        currentOffset = exe.tellg();
    }
    exe.close();
    return true;
}

void VFS::init(bool loadPackage){
    if (loadPackage && loadPackageData()){
        packaged = true;
    }else{
        packaged = false;
    }

    #ifdef _WIN32
        userPath = std::string(getenv("APPDATA")) + "Bussin2D/";
    #else
        userPath = std::string(getenv("HOME")) + "/.local/share/Bussin2D";
    #endif
    fs::create_directory(userPath);
}

std::string VFS::resolvePath(const std::string& path){
    if (path.substr(0, 6) == "res://"){
        return path.substr(6);
    }
    if (path.substr(0, 7) == "user://"){
        return userPath +  path.substr(7);
    }
    return path;
}

bool VFS::writeText(const std::string& path, const std::string& content){
    std::string fullPath = VFS::resolvePath(path);
    if (path.substr(0, 7) == "user://"){
        std::cerr << Colors::RED << "Cannot write to read-only files" << std::endl;
        return false;
    }
    std::ofstream f(fullPath);
    if (!f.is_open()) return false;

    f << content;

    f.close();

    return true;
}

bool VFS::writeBinary(const std::string& path, const std::vector<unsigned char>& data){
    std::string fullPath = VFS::resolvePath(path);
    if (path.substr(0, 7) == "user://"){
        std::cerr << Colors::RED << "Cannot write to read-only files" << std::endl;
        return false;
    }

    std::ofstream f(fullPath, std::ios::binary);
    if (!f.is_open()) return false;
    
    f.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    f.close();

    return true;
}

std::string VFS::readText(const std::string& path){
    if (packaged){
        auto data = loadFileData(path);
        return std::string(data.begin(), data.end());
    }

    std::ifstream f(path);
    if (!f.is_open()) return "";
    return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

std::vector<unsigned char> VFS::readBinary(const std::string& path){
    if (packaged){
        return loadFileData(path);
    }
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) return {};

    size_t size = f.tellg();
    f.seekg(0);
    std::vector<unsigned char> data(size);
    f.read((char*)data.data(), size);
    return data;
}

bool VFS::exists(const std::string& path){
    if (packaged) return files.find(path) != files.end();
    return fs::exists(path);
}

std::vector<unsigned char> VFS::loadFileData(const std::string& path){
    auto it = files.find(path);
    if (it == files.end()){
        return {};
    }
    FileEntry& entry = it->second;
    if (entry.loaded){
        return entry.data;
    }

    std::ifstream exe(exePath, std::ios::binary);
    if (!exe.is_open()){
        std::cerr << "[VFS] Failed to open exe for reading" << std::endl;
        return {};
    }

    exe.seekg(entry.offset);
    entry.data.resize(entry.size);
    exe.read((char*)entry.data.data(), entry.size);
    entry.loaded = true;

    return entry.data;
}

