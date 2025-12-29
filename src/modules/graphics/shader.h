#pragma once
#include <string>
#include "glad.h"
#include <unordered_map>

class Shader{
public:
    GLuint program;

    Shader();
    ~Shader();
    
    GLuint compileShader(const std::string& source, GLenum type);
    std::string loadFile(const std::string& path);

    bool loadFromFiles(const std::string& vertPath,const std::string& fragPath);
    void use();

    void setFloat(const char* name, float value);
    void setVec2(const char* name, float x, float y);
    void setVec3(const char* name, float x, float y, float z);
    void setVec4(const char* name, float x, float y, float z, float w);
    void setInt(const char* name, int value);
    void setMat4(const char* name, const float* value);
};
