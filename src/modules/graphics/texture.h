#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/color.h"
#include "glad.h"
#include "renderer.h"

class Texture{
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& path);
    void bind();

    void setTransparency(Uint8 newTransparency) {
        transparency = newTransparency;
    }

    int getTransparency() const { return transparency; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool isLoaded() const { return textureID != 0; }
    GLuint getID() const { return textureID; }
    
    std::string createBMP(int width, int height, std::string imgID = "");
    bool changePixel(int x, int y, RGBA color);
    bool updateBMPTexture();
private:
    GLuint textureID;
    int width;
    int height;
    Uint8 transparency;
    bool isBMP;
    std::string bmpID;
};