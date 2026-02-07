#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

#include "core/common.h"
#include "build.h"

#ifdef _WIN32
#include <windows.h>
std::string getCurrentExeName() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return std::string(path);
}
#else
#include <unistd.h>
#include <limits.h>
std::string getCurrentExeName() {
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    return std::string(path, (count > 0) ? count : 0);
}
#endif

namespace fs = std::filesystem;

std::vector<std::string> findFiles(){
    std::vector<std::string> files;

    for (auto& entry: fs::recursive_directory_iterator(".")){
        if (entry.is_regular_file()){
            std::string ext = entry.path().extension().string();
            // we should check all formats in an array or smth
            if (ext == ".lua" || ext == ".png" || ext == ".wav" || ext == ".ttf" || ext == ".dll"  || ext == ".so"  || ext == ".mp3"){
                files.push_back(entry.path().string());
            }
        }
    }

    return files;
}

int buildProject(const std::string& platform){
    std::cout << "Building for " << platform << std::endl;

    std::string cwd = fs::current_path().string();

    auto files = findFiles();
    if (files.empty()){
        std::cerr << "No files found" << std::endl;
        return 1;
    }

    std::string exeExt = "";
    std::string templateName = "";

    if (platform == "windows"){
        exeExt += ".exe";
    }else if (platform == "linux"){
        // it wont compile to linux, i am too fucking lazy to do this, may do it in the future
        exeExt += "";
    }else{
        std::cerr << "Unknown platform: " << platform << std::endl;
        return 1;
    }

    templateName = "template_" + platform + exeExt;

    fs::path exeName = getCurrentExeName();
    fs::path engineDir = fs::path(exeName).parent_path();
    fs::path templatePath = engineDir / templateName;
    
    if (!fs::exists(templatePath)) {
        std::cerr << "Template not found: " << templatePath << std::endl;
        return 1;
    }

    fs::path buildDir = fs::absolute("build");
    fs::create_directories(buildDir);

    std::string outPath = "build/game" + exeExt;

    try{
        fs::path srcPath = fs::absolute(templatePath);
        fs::path dstPath = fs::absolute(outPath);

        fs::copy_file(srcPath, dstPath, fs::copy_options::overwrite_existing);

        auto copiedSize = fs::file_size(dstPath);
        std::cout << Colors::BLUE << " Copied exe size: " << copiedSize << " bytes" << std::endl;

        #ifndef _WIN32
            chmod(dstPath.c_str(), 0755);
        #endif
    } catch (std::exception& e){
        std::cerr << Colors::RED << " Copy failed: " << e.what() << std::endl;
        return 1;
    }

    #ifdef _WIN32
    if (platform == "windows") {
        std::cout << Colors::CYAN << "Copying dependencies:" << std::endl;
        
        try {
            for (auto& entry : fs::directory_iterator(engineDir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    std::string ext = entry.path().extension().string();
                    
                    if (ext == ".dll") {
                        fs::path destFile = buildDir / filename;
                        fs::copy_file(entry.path(), destFile, fs::copy_options::overwrite_existing);
                        std::cout << "  Copied: " << filename << std::endl;
                    }
                }
            }
        } catch (std::exception& e) {
            std::cerr << Colors::YELLOW << "Warning: Failed to copy some dependencies: " << e.what() << std::endl;
        }
    }
    #endif

    uint64_t offset = fs::file_size(outPath);
    
    std::ofstream out(outPath, std::ios::binary | std::ios::app);
    if (!out.is_open()) {
        std::cerr << Colors::RED << "Failed to open output file for packaging!" << std::endl;
        return 1;
    }
    
    uint32_t count = files.size();
    out.write((char*)&count, sizeof(count));
    
    std::cout << "Packaging " << count << " files..." << std::endl;
    
    for (auto& path : files) {
        std::string p = path;
        if (p[0] == '.' && (p[1] == '/' || p[1] == '\\')) {
            p = p.substr(2);
        }
        for (char& c : p) {
            if (c == '\\') c = '/';
        }
        
        std::ifstream in(path, std::ios::binary | std::ios::ate);
        if (!in.is_open()) {
            std::cerr << Colors::YELLOW << "  WARNING: Failed to open: " << path << std::endl;
            continue;
        }
        
        uint32_t size = in.tellg();
        in.seekg(0);
        
        uint32_t len = p.length();
        out.write((char*)&len, sizeof(len));
        out.write(p.c_str(), len);
        
        out.write((char*)&size, sizeof(size));
        
        std::vector<char> data(size);
        in.read(data.data(), size);
        out.write(data.data(), size);
        
        std::cout << "  " << p << " (" << size << " bytes)" << std::endl;
    }
    
    struct Footer {
        uint64_t offset;
        uint64_t size;
        char magic[7] = "BUSSIN";
    };
    
    Footer footer;
    footer.offset = offset;
    footer.size = static_cast<uint64_t>(fs::file_size(outPath)) - offset;
    out.write((char*)&footer, sizeof(footer));
    out.close();
    
    std::cout << Colors::GREEN << "Writing footer: offset=" << footer.offset 
            << ", size=" << footer.size << std::endl;
    std::cout << Colors::GREEN << "Build: " << outPath << std::endl;
    std::cout << Colors::RESET << std::endl;
    return 0;
}