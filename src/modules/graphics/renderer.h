#pragma once

#include <sdl2/SDL.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "texture.h"

class Renderer{
public:
    Renderer(SDL_Renderer* sdlRenderer);
    ~Renderer();

    void drawRect(
        int x,
        int y,
        int w,
        int h,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a = 255
    );
    void drawRectFilled(
        int x,
        int y,
        int w,
        int h,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a = 255
    );
    void drawTriangle(
        int x1,
        int y1,
        int x2,
        int y2,
        int x3,
        int y3,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a = 255
    );
    void drawCircle(
        int centerX,
        int centerY,
        int radius,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a = 255
    );
    void drawLine(
        int x1,
        int y1,
        int x2,
        int y2,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a = 255
    );

    int loadTexture(const std::string& path);
    void drawTexture(int textureId, int x, int y, int w = -1, int h = -1);
private:
    SDL_Renderer* renderer;
    std::unordered_map<int, std::shared_ptr<Texture>> textures;
    int nextTextureId;
};