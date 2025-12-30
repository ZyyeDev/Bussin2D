#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "glad.h"
#include "stb_truetype.h"

struct Character {
    GLuint textureID;
    int width, height;
    int bearingX, bearingY;
    int advance;
};

class Font{
    public:
        Font();
        ~Font();
        bool loadFromFile(const std::string& path, int fontSize);
        Character* getCharacter(char c);
        int getSize() const { return fontSize; }
    private:
        std::unordered_map<char, Character> characters;
        int fontSize;
};
