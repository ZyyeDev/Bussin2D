#include "texture.h"
#include "core/common.h"
#include <SDL2/SDL_image.h>

Texture::Texture() {
    texture = nullptr;
    width = 0;
    height = 0;
};

Texture::~Texture(){
    if (texture){
        SDL_DestroyTexture(texture);
    }
}

bool Texture::loadFromFile(const std::string& path, SDL_Renderer* renderer){
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface){
        std::cerr << "Failed to load img: " << path << " " << IMG_GetError() << std::endl;
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture){
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }

    width = surface->w;
    height = surface->h;

    SDL_FreeSurface(surface);
    return true;
}

void Texture::render(SDL_Renderer* renderer, int x, int y, int w, int h){
    if (!texture) return;

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    if (w != -1){
        rect.w = w;
    }else{
        // use original width if not specified
        rect.w = width;
    }
    if (w != -1){
        rect.h = h;
    }else{
        // use original height if not specified
        rect.h = height;
    }

    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}