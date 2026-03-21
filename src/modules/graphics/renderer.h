#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#include "shader.h"
//#include "glad.h"
#include "texture.h"
#include "font.h"

struct drawcallData{
    int type = GL_TRIANGLES;
    std::vector<float> batch;
};

struct BMPData {
    int width, height;

    Texture* textureID;
    bool dirty = true;

    std::vector<RGBA> pixelData;
};

extern std::unordered_map<std::string, BMPData> BMPCache;

class Renderer{
public:
    Renderer(int width, int height);
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
    void drawLine(
        int x1,
        int y1,
        int x2,
        int y2,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a
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

    int loadTexture(const std::string& path);
    void drawTexture(int textureId, int x, int y, int h = -1, int w = -1);
    void setTransparency(int textureId, Uint8 transparency);

    int loadShader(const std::string& vertPath, const std::string& fragPath);
    void useShader(int shaderId);
    void setShaderFloat(int shaderId, const char* name, float value);
    void setShaderVec2(int shaderId, const char* name, float x, float y);
    void setShaderVec3(int shaderId, const char* name, float x, float y, float z);
    void setShaderVec4(int shaderId, const char* name, float x, float y, float z, float w);
    void setShaderInt(int shaderId, const char* name, int value);
    
    void createProjectionMatrix(int width, int height);

    int loadFont(const std::string& path, int size);
    void drawText(int fontId, const std::string text, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    GLuint getBMPTexture(const std::string& imgID);
    int createBMP(int width, int height, const std::string textureId);
    bool editPixelBMP(int x, int y, RGBA color, const int textureId);

    int getNextTextureId() { return nextTextureId; }
    
    void addToBatch(std::vector<float>&& data, int type);

    void flush();

    void setVirtualResolution(int w, int h);
    void clearVirtualResolution();
    bool hasVirtualResolution() const { return virtualResEnabled; }
    int getVirtualWidth() const { return virtualWidth; }
    int getVirtualHeight() const { return virtualHeight; }
    
    void setBaseTextureShader(int id);

    void beginVirtualFrame();
    void endVirtualFrame(int windowW, int windowH);
private:
    GLuint VAO, VBO;
    GLuint textureVAO, textureVBO;

    int viewWidth, viewHeight;
    std::shared_ptr<Shader> defaultShader;
    std::shared_ptr<Shader> currentShader;
    std::shared_ptr<Shader> textureShader;
    std::shared_ptr<Shader> baseTextureShader;
    std::unordered_map<int, std::shared_ptr<Shader>> shaders;
    int nextShaderId;

    float projectionMatrix[16];

    SDL_Renderer* renderer;
    std::unordered_map<int, std::shared_ptr<Texture>> textures;
    int nextTextureId;
    
    std::unordered_map<int, std::shared_ptr<Font>> fonts;
    int nextFontId = 1;
    std::shared_ptr<Shader> textShader;

    std::vector<drawcallData> drawcallBatch;
    std::vector<float> mergedBuffer;

    // Virtual resolution FBO
    bool virtualResEnabled = false;
    int virtualWidth = 0;
    int virtualHeight = 0;
    GLuint fbo = 0;
    GLuint fboTexture = 0;
    GLuint fboRBO = 0;
    GLuint blitVAO = 0;
    GLuint blitVBO = 0;
    std::shared_ptr<Shader> blitShader;
    void initBlitResources();
    void destroyFBO();
};