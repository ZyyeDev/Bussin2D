#include <unordered_map>
#include <map>
#include <SDL2/SDL_image.h>

#include "renderer.h"
#include "texture.h"
#include "core/common.h"
#include "variables.h"

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

bool Texture::loadFromFile(const std::string& path){
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

std::string Texture::createBMP(int BMPwidth, int BMPheight, std::string imgID){
    if (!g_renderer){
        return "";
    }

    if (imgID != "" && BMPCache.find(imgID) == BMPCache.end()){
        
    }else{
        // very cool thingy (ik this is horrible)
        imgID = std::to_string(CURRENT_TICK+BMPwidth)+std::to_string(BMPheight)+"_"+std::to_string(textureID);
    }
    imgID = "bmp_"+imgID;
    std::cout << imgID<<std::endl;

    isBMP = true;
    bmpID = imgID;

    BMPData data;
    data.width = BMPwidth;
    data.height = BMPheight;
    data.pixelData.resize(BMPwidth*BMPheight);
    data.dirty = true;

    width = BMPwidth;
    height = BMPheight;

    for (int y = 0; y < BMPheight; y++){
        for (int x = 0; x < BMPwidth; x++){
            RGBA color;
            color.R = 0;
            color.G = 0;
            color.B = 0;
            color.A = 0;
            data.pixelData[y*BMPwidth + x] = color;
        }
    }

    BMPCache[imgID] = data;

    textureID = g_renderer->getNextTextureId();

    std::cout << "tex " << (textureID) << std::endl;

    if (textureID != 0){
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_BGRA,
            BMPwidth,
            height,
            0,
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            data.pixelData.data()
        );

        data.dirty = false;
    }else{
        std::cerr << "TEXTURE ID IS 0!!!!!!" << std::endl;
    }

    return imgID;
}

bool Texture::changePixel(int x, int y, RGBA color){
    if (!isBMP) return false;
    
    if (bmpID == ""){
        std::cerr << "BMP is null" << std::endl;
        return false;
    }
    auto it = BMPCache.find(bmpID);
    if (it == BMPCache.end()){
        std::cerr << "BMP image does not exist" << std::endl;
        return false;
    }
    auto& data = it->second;
    if (x < 0 || x >= data.width || y < 0 || y >= data.height) {
        std::cerr << "BMP Out of bounds!" << std::endl;
        return false; 
    }

    data.pixelData[y*data.width+x] = color;
    data.dirty = true;
    return true;
}

bool Texture::updateBMPTexture(){
    if (!isBMP){
        return false;
    }
    
    auto it = BMPCache.find(bmpID);
    if (it == BMPCache.end()){
        return false;
    }
    
    BMPData& bmp = it->second;

    if (bmp.dirty){
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_BGRA,
            bmp.width,
            bmp.height,
            0,
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            bmp.pixelData.data()
        );

        bmp.dirty = false;
        return true;
    }

    return false;
}