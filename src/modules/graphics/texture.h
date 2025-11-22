#pragma once

#include <SDL2/SDL.h>
#include <string>

class Texture{
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& path, SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer, int x, int y, int w = -1, int h = -1);

    int getWidth() const { return width; }
    int getHeight() const {return height; }
    bool isLoaded() const { return texture != nullptr; }
private:
    SDL_Texture* texture;
    int width;
    int height;
};