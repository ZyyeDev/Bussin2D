#include "renderer.h"
#include "core/common.h"
#include <cmath>

Renderer::Renderer(SDL_Renderer* sdlRenderer){
    renderer = sdlRenderer;
    nextTextureId = 1;
};

Renderer::~Renderer(){
    textures.clear();
}

void Renderer::drawRect(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    SDL_SetRenderDrawColor(renderer, r,g,b,a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

void Renderer::drawRectFilled(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    SDL_SetRenderDrawColor(renderer, r,g,b,a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    SDL_SetRenderDrawColor(renderer, r,g,b,a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Renderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    SDL_SetRenderDrawColor(renderer, r,g,b,a);

    SDL_RenderDrawLine(renderer, x1,y1, x2,y2);
    SDL_RenderDrawLine(renderer, x2,y2, x3,y3);
    SDL_RenderDrawLine(renderer, x3,y3, x1,y1);
}

void Renderer::drawCircle(int centerX, int centerY, int radius, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    
    // Bresenham's circle algorithm
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
        
        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}
int Renderer::loadTexture(const std::string& path){
    auto texture = std::make_shared<Texture>();

    auto loaded = texture->loadFromFile(path,renderer);
    if (!loaded){
        std::cerr << "Failed to load texture";
        return 1;
    }

    int id = nextTextureId++;
    textures[id] = texture;
    return id;
}

void Renderer::drawTexture(int textureId, int x, int y, int h, int w){
    auto it = textures.find(textureId);
    if (it == textures.end()){
        std::cerr << "Texture id " << textureId << " not found" << std::endl;
        return;
    }
    it->second->render(renderer, x, y, h, w);
}