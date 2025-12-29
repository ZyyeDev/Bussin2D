#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader() : program(0) {}

Shader::~Shader() {
    if (program) glDeleteProgram(program);
}

std::string Shader::loadFile(const std::string& path){
    std::ifstream file(path);
    if (!file.is_open()){
        std::cerr << "Failed to open shader: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(const std::string& source, GLenum type){
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success){
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation failed: " << log << std::endl;
        return 0;
    }
    return shader;
}

bool Shader::loadFromFiles(const std::string& vertPath, const std::string& fragPath){
    std::string vertSource = loadFile(vertPath);
    std::string fragSource = loadFile(fragPath);

    if (vertSource.empty() || fragPath.empty()){
        return false;
    }

    GLuint vertShader = compileShader(vertSource, GL_VERTEX_SHADER);
    GLuint fragShader = compileShader(fragSource, GL_FRAGMENT_SHADER);

    if (!vertShader || !fragShader){
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success){
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Shader linking failed: " << log << std::endl;
        return false;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return true;
}

void Shader::use(){
    glUseProgram(program);
}

void Shader::setFloat(const char* name, float value) {
    glUniform1f(glGetUniformLocation(program, name), value);
}
void Shader::setVec2(const char* name, float x, float y) {
    glUniform2f(glGetUniformLocation(program, name), x, y);
}
void Shader::setVec3(const char* name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(program, name), x,y,z);
}
void Shader::setVec4(const char* name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(program, name), x,y,z,w);
}
void Shader::setInt(const char* name, int value) {
    glUniform1i(glGetUniformLocation(program, name), value);
}
void Shader::setMat4(const char* name, const float* value) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, value);
}
