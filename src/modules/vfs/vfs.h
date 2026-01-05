#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

struct FileEntry{
    uint64_t offset;
    uint32_t size;
    bool loaded;
    std::vector<unsigned char> data;
};

class VFS{
public:
    static VFS& get();

    void init(bool loadPackage);
    bool isPackaged() const {return packaged;}

    std::string readText(const std::string& path);
    std::vector<unsigned char> readBinary(const std::string& path);
    std::vector<unsigned char> loadFileData(const std::string& path);
    bool exists(const std::string& path);
private:
    VFS() = default;
    bool packaged = false;
    std::unordered_map<std::string, FileEntry> files;
    std::string exePath;

    bool loadPackageData();
    std::string getExePath();
};