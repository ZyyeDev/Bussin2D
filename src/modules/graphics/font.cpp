#include "font.h"

#include <fstream>
#include <vector>
#include <iostream>
#include "modules/vfs/vfs.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

Font::Font() : fontSize(0) {};

Font::~Font(){
    for (auto& pair : characters){
        glDeleteTextures(1, &pair.second.textureID);
    }
}

bool Font::loadFromFile(const std::string& path, int size){
    fontSize = size;

    auto buffer = VFS::get().readBinary(path);
    
    // init truetype
    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, buffer.data(), 0)){
        std::cerr << "Failed to load font " << path << std::endl;
        return false;
    }

    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);

    // load ascii chars
    for (unsigned char c = 32; c < 128; c++){
        int width, height, xoff, yoff;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, c, &width, &height, &xoff, &yoff);

        if (!bitmap) continue;

        // create opengl texture
        GLuint texture;
        if (bitmap && width > 0 && height > 0) {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            
            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED,
                width, height, 0,
                GL_RED, GL_UNSIGNED_BYTE, bitmap
            );
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, c, &advance, &lsb);

        Character charact = {
            texture,
            width,
            height,
            xoff,
            yoff,
            (int)(advance * scale)
        };

        characters[c] = charact;
        stbtt_FreeBitmap(bitmap, nullptr);
    }

    return true;
}

Character* Font::getCharacter(char c){
    auto it = characters.find(c);
    if (it != characters.end()){
        return &it->second;
    }
    return nullptr;
}