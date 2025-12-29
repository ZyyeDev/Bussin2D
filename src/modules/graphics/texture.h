#pragma once

#include <SDL2/SDL.h>
#include <string>
#include "glad.h"

class Texture{
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& path, SDL_Renderer* renderer);
    void bind();

    void setTransparency(Uint8 newTransparency) {
        transparency = newTransparency;
    }

    int getTransparency() const { return transparency; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isLoaded() const { return textureID != 0; }
    GLuint getID() const { return textureID; }
private:
    GLuint textureID;
    int width;
    int height;
    Uint8 transparency;
};