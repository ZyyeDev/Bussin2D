#include "texture.h"
#include "core/common.h"
#include <SDL2/SDL_image.h>

Texture::Texture() {
    textureID = 0;
    width = 0;
    height = 0;
    transparency = 1.0f;
};

Texture::~Texture(){
    if (textureID){
        glDeleteTextures(1, &textureID);
    }
}

bool Texture::loadFromFile(const std::string& path, SDL_Renderer* renderer){
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface){
        std::cerr << "Failed to load img: " << path << " " << IMG_GetError() << std::endl;
        return false;
    }

    width = surface->w;
    height = surface->h;

    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    if (!formattedSurface){
        std::cerr << "Failed to convert surface format: " << SDL_GetError() << std::endl;
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        formattedSurface->pixels
    );

    SDL_FreeSurface(surface);
    return true;
}

void Texture::bind(){
    glBindTexture(GL_TEXTURE_2D, textureID);
}