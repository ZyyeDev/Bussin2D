#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class VFS{
public:
    static VFS& get();

    void init(bool loadPackage);
    bool isPackaged() const {return packaged;}

    std::string readText(const std::string& path);
    std::vector<unsigned char> readBinary(const std::string& path);
    bool exists(const std::string& path);
private:
    VFS() = default;
    bool packaged = false;
    std::unordered_map<std::string, std::vector<unsigned char>> files;

    bool loadPackageData();
    std::string getExePath();
};