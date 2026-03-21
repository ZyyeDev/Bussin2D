#include "renderer.h"
#include "core/common.h"
#include <cmath>

#include "variables.h"

std::unordered_map<std::string, BMPData> BMPCache;

Renderer::Renderer(int width, int height){
    nextTextureId = 1;

    drawcallBatch.reserve(10000);
    mergedBuffer.reserve(65536);

    viewWidth = width;
    viewHeight = height;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    defaultShader = std::make_shared<Shader>();

    std::string vertSource = R"(
        #version 330 core
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec4 color;
        out vec4 fragColor;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * vec4(position, 0.0, 1.0);
            fragColor = color;
        }
    )";

    std::string fragSource = R"(
        #version 330 core
        in vec4 fragColor;
        out vec4 outColor;
        void main() {
            outColor = fragColor;
        }
    )";

    // text stuff
    textShader = std::make_shared<Shader>();
    std::string textVertSource = R"(
        #version 330 core
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 texCoord;
        out vec2 fragTexCoord;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * vec4(position, 0.0, 1.0);
            fragTexCoord = texCoord;
        }
    )";
    std::string textFragSource = R"(
        #version 330 core
        in vec2 fragTexCoord;
        out vec4 outColor;
        uniform sampler2D textureSampler;
        uniform vec4 textColor;
        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureSampler, fragTexCoord).r);
            outColor = textColor * sampled;
        }
    )";

    GLuint textVertShader = textShader->compileShader(textVertSource, GL_VERTEX_SHADER);
    GLuint textFragShader = textShader->compileShader(textFragSource, GL_FRAGMENT_SHADER);

    textShader->program = glCreateProgram();
    glAttachShader(textShader->program, textVertShader);
    glAttachShader(textShader->program, textFragShader);
    glLinkProgram(textShader->program);

    GLuint vertShader = defaultShader->compileShader(vertSource, GL_VERTEX_SHADER);
    GLuint fragShader = defaultShader->compileShader(fragSource, GL_FRAGMENT_SHADER);

    defaultShader->program = glCreateProgram();
    glAttachShader(defaultShader->program, vertShader);
    glAttachShader(defaultShader->program, fragShader);
    glLinkProgram(defaultShader->program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // texture stuff
    glGenVertexArrays(1, &textureVAO);
    glGenBuffers(1, &textureVBO);

    glBindVertexArray(textureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

    // text stuf
    glDeleteShader(textVertShader);
    glDeleteShader(textFragShader);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture shader
    textureShader = std::make_shared<Shader>();

    std::string texVertSource = R"(
        #version 330 core
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 texCoord;
        out vec2 fragTexCoord;
        uniform mat4 projection;
        void main() {
            gl_Position = projection * vec4(position, 0.0, 1.0);
            fragTexCoord = texCoord;
        }
    )";

    std::string texFragSource = R"(
        #version 330 core
        in vec2 fragTexCoord;
        out vec4 outColor;
        uniform sampler2D textureSampler;
        uniform float alpha;
        void main() {
            vec4 texColor = texture(textureSampler, fragTexCoord);
            outColor = vec4(texColor.rgb, texColor.a * alpha);
        }
    )";

    GLuint texVertShader = textureShader->compileShader(texVertSource, GL_VERTEX_SHADER);
    GLuint texFragShader = textureShader->compileShader(texFragSource, GL_FRAGMENT_SHADER);

    textureShader->program = glCreateProgram();
    glAttachShader(textureShader->program, texVertShader);
    glAttachShader(textureShader->program, texFragShader);
    glLinkProgram(textureShader->program);

    glDeleteShader(texVertShader);
    glDeleteShader(texFragShader);

    currentShader = defaultShader;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
};

Renderer::~Renderer(){
    flush();
    destroyFBO();

    if (blitVAO){
        glDeleteVertexArrays(1, &blitVAO); blitVAO = 0;
    }

    if (blitVBO){
        glDeleteBuffers(1, &blitVBO);
        blitVBO = 0;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteBuffers(1, &textureVBO);

    textures.clear();
}

void Renderer::createProjectionMatrix(int width, int height) {
    if (!g_camera) init_camera();

    // HACK: after adding camera the engine kept crashing, this fixes it
    if (!g_camera || !g_window){
        return;
    }
    float left = g_camera->getX();
    float right = g_camera->getX() + (float)width / g_camera->getZoom();
    float bottom = g_camera->getY() + (float)height / g_camera->getZoom();
    float top = g_camera->getY();
    float near = -1.0f;
    float far = 1.0f;
    
    //std::cout << left << " " << right << std::endl;
    
    projectionMatrix[0] = 2.0f / (right - left);
    projectionMatrix[1] = 0.0f;
    projectionMatrix[2] = 0.0f;
    projectionMatrix[3] = 0.0f;
    
    projectionMatrix[4] = 0.0f;
    projectionMatrix[5] = 2.0f / (top - bottom);
    projectionMatrix[6] = 0.0f;
    projectionMatrix[7] = 0.0f;
    
    projectionMatrix[8] = 0.0f;
    projectionMatrix[9] = 0.0f;
    projectionMatrix[10] = -2.0f / (far - near);
    projectionMatrix[11] = 0.0f;
    
    projectionMatrix[12] = -(right + left) / (right - left);
    projectionMatrix[13] = -(top + bottom) / (top - bottom);
    projectionMatrix[14] = -(far + near) / (far - near);
    projectionMatrix[15] = 1.0f;

    viewWidth = width;
    viewHeight = height;
}

void Renderer::drawRect(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;
    float fa = a / 255.0f;
    
    addToBatch(
    std::vector<float>{
        (float)x, (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)(y+h), fr, fg, fb, fa,
        
        (float)x, (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)(y+h), fr, fg, fb, fa,
        (float)x, (float)(y+h), fr, fg, fb, fa,
    }, GL_TRIANGLES);
}

void Renderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;
    float fa = a / 255.0f;
    
    addToBatch(
    std::vector<float>{
        (float)x1, (float)y1, fr, fg, fb, fa,
        (float)x2, (float)y2, fr, fg, fb, fa,
        (float)x3, (float)y3, fr, fg, fb, fa,
    }, GL_TRIANGLES);
}

void Renderer::drawCircle(int centerX, int centerY, int radius, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;
    float fa = a / 255.0f;
    
    const int segments = 32;
    
    for (int i = 0; i < segments; i++) {
        float angle1 = (i / (float)segments) * 2.0f * M_PI;
        float angle2 = ((i + 1) / (float)segments) * 2.0f * M_PI;

        float x1 = centerX + cos(angle1) * radius;
        float y1 = centerY + sin(angle1) * radius;
        float x2 = centerX + cos(angle2) * radius;
        float y2 = centerY + sin(angle2) * radius;

        addToBatch(
        std::vector<float>{
            (float)centerX, (float)centerY, fr, fg, fb, fa,
            x1, y1, fr, fg, fb, fa,
            x2, y2, fr, fg, fb, fa
        }, GL_TRIANGLES);
    }
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;
    float fa = a / 255.0f;
    
    addToBatch(
    std::vector<float>{
        (float)x1, (float)y1, fr, fg, fb, fa,
        (float)x2, (float)y2, fr, fg, fb, fa,
    }, GL_LINES);
}

int Renderer::loadTexture(const std::string& path){
    auto texture = std::make_shared<Texture>();

    auto loaded = texture->loadFromFile(path);
    if (!loaded){
        std::cerr << "Failed to load texture" << std::endl;
        return -1;
    }

    int id = nextTextureId++;
    textures[id] = texture;
    return id;
}

int Renderer::createBMP(int width, int height, const std::string textureId){
    auto texture = std::make_shared<Texture>();

    auto ctex = texture->createBMP(width, height, textureId);
    auto loaded = texture->updateBMPTexture();
    if (ctex == ""){
        std::cerr << "Failed to create BMP" << std::endl;
        return -1;
    }

    int id = nextTextureId++;
    textures[id] = texture;
    return id;
}

bool Renderer::editPixelBMP(int x, int y, RGBA color, const int textureId){
    auto it = textures.find(textureId);
    if (it == textures.end()){
        return false;
    }
    auto texture = it->second;

    if (!texture->changePixel(x, y, color)) {
        return false;
    }

    if (!texture->updateBMPTexture()){ 
        return false; 
    }

    return true;
}

void Renderer::setBaseTextureShader(int id){
    auto it = shaders.find(id);
    
    if (it != shaders.end()){
        baseTextureShader = it->second;
    }
}

void Renderer::drawTexture(int textureId, int x, int y, int h, int w){
    auto it = textures.find(textureId);
    if (it == textures.end()){
        std::cerr << "Texture id " << textureId << " not found" << std::endl;
        return;
    }
    auto texture = it->second;

    // use original
    if (w == -1) w = texture->getWidth();
    if (h == -1) h = texture->getHeight();

    float x2 = x + w;
    float y2 = y + h;

    float vertices[] = {
        (float)x,  (float)y,  0.0f, 0.0f,  // top left
        (float)x2, (float)y,  1.0f, 0.0f,  // top right
        (float)x2, (float)y2, 1.0f, 1.0f,  // bottom right
        
        (float)x,  (float)y,  0.0f, 0.0f,  // top left
        (float)x2, (float)y2, 1.0f, 1.0f,  // bottom right
        (float)x,  (float)y2, 0.0f, 1.0f,  // bottom left
    };

    std::shared_ptr<Shader> shaderToUse;
    if (currentShader != defaultShader){
        // custom shader
        shaderToUse = currentShader;
    } else if (baseTextureShader){
        // atlas-slice shader
        shaderToUse = baseTextureShader;
    } else {
        // plain texture
        shaderToUse = textureShader;
    }

    shaderToUse->use();

    shaderToUse->setMat4("projection", projectionMatrix);
    shaderToUse->setFloat("alpha", texture->getTransparency());
    shaderToUse->setInt("textureSampler", 0);

    shaderToUse->setMat4("u_transform", projectionMatrix);
    shaderToUse->setInt("u_texture", 0);
    //shaderToUse->setVec4("u_quad_rect", 0.0f, 0.0f, 1.0f, 1.0f);
    shaderToUse->setVec2("u_resolution", (float)viewWidth, (float)viewHeight);

    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    glBindVertexArray(textureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::setTransparency(int textureId, Uint8 transparency){
    auto it = textures.find(textureId);
    if (it == textures.end()){
        std::cerr << "Texture id " << textureId << " not found" << std::endl;
        return;
    }
    it->second->setTransparency(transparency/255.0f);
}

int Renderer::loadShader(const std::string& vertPath, const std::string& fragPath){
    auto shader = std::make_shared<Shader>();

    if (!shader->loadFromFiles(vertPath, fragPath)){
        std::cerr << "Failed to load shader!" << std::endl;
        return -1;
    }

    int id = nextShaderId++;
    shaders[id] = shader;
    return id;
}

void Renderer::useShader(int shaderId){
    if (shaderId == 0){
        currentShader = defaultShader;
        return;
    }
    auto it = shaders.find(shaderId);
    if (it != shaders.end()){
        currentShader = it->second;
    }
}

void Renderer::setShaderFloat(int shaderId, const char* name, float value) {
    auto it = shaders.find(shaderId);
    if (it != shaders.end()) {
        it->second->use();
        it->second->setFloat(name, value);
    }
}

void Renderer::setShaderVec2(int shaderId, const char* name, float x, float y) {
    auto it = shaders.find(shaderId);
    if (it != shaders.end()) {
        it->second->use();
        it->second->setVec2(name, x, y);
    }
}

void Renderer::setShaderVec3(int shaderId, const char* name, float x, float y, float z) {
    auto it = shaders.find(shaderId);
    if (it != shaders.end()) {
        it->second->use();
        it->second->setVec3(name, x, y, z);
    }
}

void Renderer::setShaderVec4(int shaderId, const char* name, float x, float y, float z, float w) {
    auto it = shaders.find(shaderId);
    if (it != shaders.end()) {
        it->second->use();
        it->second->setVec4(name, x, y, z, w);
    }
}

void Renderer::setShaderInt(int shaderId, const char* name, int value) {
    auto it = shaders.find(shaderId);
    if (it != shaders.end()) {
        it->second->use();
        it->second->setInt(name, value);
    }
}

int Renderer::loadFont(const std::string& path, int size){
    auto font = std::make_shared<Font>();
    
    if (!font->loadFromFile(path, size)){
        std::cerr << "Failed to load font " << path << std::endl;
        return -1;
    }

    int id = nextFontId++;
    fonts[id] = font;
    std::cout << id << std::endl;
    std::cout << font << std::endl;
    return id;
}

void Renderer::drawText(int fontId, const std::string text, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    auto it = fonts.find(fontId);
    if (it == fonts.end()){
        return;
    }

    auto font = it->second;

    float fr = r/255.0f;
    float fg = g/255.0f;
    float fb = b/255.0f;
    float fa = a/255.0f;

    textShader->use();
    textShader->setMat4("projection", projectionMatrix);
    textShader->setVec4("textColor", fr, fg, fb, fa);
    textShader->setInt("textureSampler", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textureVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    float currentX = x;

    for (char c : text){
        Character* ch = font->getCharacter(c);
        if (!ch && c == ' '){
            // hacky hacky
            ch = font->getCharacter('O');
            if (!ch) continue;
            currentX += ch->advance;
            continue;
        }
        if (!ch) continue;
        
        float xpos = currentX + ch->bearingX;
        float ypos = y - (ch->height - ch->bearingY);

        float w = ch->width;
        float h = ch->height;

        float vertices[] = {
            xpos,     ypos,     0.0f, 0.0f,  // top left
            xpos + w, ypos,     1.0f, 0.0f,  // top right
            xpos + w, ypos + h, 1.0f, 1.0f,  // bottom right
            
            xpos,     ypos,     0.0f, 0.0f,  // top left
            xpos + w, ypos + h, 1.0f, 1.0f,  // bottom right
            xpos,     ypos + h, 0.0f, 1.0f,  // bottom left
        };
        
        glBindTexture(GL_TEXTURE_2D, ch->textureID);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentX += ch->advance;
    }
}

void Renderer::flush() {
    if (drawcallBatch.empty()) return;
    
    defaultShader->use();
    defaultShader->setMat4("projection", projectionMatrix);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int i = 0;
    while (i < (int)drawcallBatch.size()) {
        int type = drawcallBatch[i].type;
        mergedBuffer.clear();

        while (i < (int)drawcallBatch.size() && drawcallBatch[i].type == type) {
            auto& b = drawcallBatch[i].batch;
            mergedBuffer.insert(mergedBuffer.end(), b.begin(), b.end());
            i++;
        }

        glBufferData(GL_ARRAY_BUFFER,
            mergedBuffer.size() * sizeof(float),
            mergedBuffer.data(),
            GL_DYNAMIC_DRAW);
        glDrawArrays(type, 0, mergedBuffer.size() / 6);
    }

    drawcallBatch.clear();
}

void Renderer::addToBatch(std::vector<float>&& data, int type) {
    drawcallData dcdat;
    dcdat.batch = std::move(data);
    dcdat.type = type;
    drawcallBatch.push_back(std::move(dcdat));
}

void Renderer::initBlitResources(){
    std::string blitVert = R"(
        #version 330 core
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 texCoord;
        out vec2 fragUV;
        void main(){
            gl_Position = vec4(position, 0.0, 1.0);
            fragUV = texCoord;
        }
    )";
    std::string blitFrag = R"(
        #version 330 core
        in vec2 fragUV;
        out vec4 outColor;
        uniform sampler2D screenTexture;
        void main(){
            outColor = texture(screenTexture, fragUV);
        }
    )";

    blitShader = std::make_shared<Shader>();
    GLuint vs = blitShader->compileShader(blitVert, GL_VERTEX_SHADER);
    GLuint fs = blitShader->compileShader(blitFrag, GL_FRAGMENT_SHADER);
    blitShader->program = glCreateProgram();
    glAttachShader(blitShader->program, vs);
    glAttachShader(blitShader->program, fs);
    glLinkProgram(blitShader->program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    float quadVerts[] = {
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 0.0f,
    };

    glGenVertexArrays(1, &blitVAO);
    glGenBuffers(1, &blitVBO);
    glBindVertexArray(blitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, blitVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Renderer::destroyFBO(){
    if (fboTexture){
        glDeleteTextures(1, &fboTexture);
        fboTexture = 0;
    }

    if (fboRBO){
        glDeleteRenderbuffers(1, &fboRBO);
        fboRBO = 0;
    }

    if (fbo){
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}

void Renderer::setVirtualResolution(int w, int h){
    destroyFBO();

    virtualWidth = w;
    virtualHeight = h;
    virtualResEnabled = true;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    glGenRenderbuffers(1, &fboRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, fboRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "Virtual resolution FBO is not complete!" << std::endl;
        virtualResEnabled = false;
        destroyFBO();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!blitShader) initBlitResources();
}

void Renderer::clearVirtualResolution(){
    destroyFBO();
    virtualResEnabled = false;
    virtualWidth = 0;
    virtualHeight = 0;
}

void Renderer::beginVirtualFrame(){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, virtualWidth, virtualHeight);
}

void Renderer::endVirtualFrame(int windowW, int windowH){
    flush();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float scaleX = (float)windowW / (float)virtualWidth;
    float scaleY = (float)windowH / (float)virtualHeight;
    float scale = 0;
    if (scaleX < scaleY){
        scale = scaleX;
    }else{
        scale = scaleY;
    }

    int scaledW = (int)(virtualWidth * scale);
    int scaledH = (int)(virtualHeight * scale);
    int offsetX = (windowW - scaledW) / 2;
    int offsetY = (windowH - scaledH) / 2;

    glViewport(0, 0, windowW, windowH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(offsetX, offsetY, scaledW, scaledH);

    blitShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTexture);

    GLint loc = glGetUniformLocation(blitShader->program, "screenTexture");
    glUniform1i(loc, 0);

    glBindVertexArray(blitVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glViewport(0, 0, windowW, windowH);
}