#include "renderer.h"
#include "core/common.h"
#include <cmath>

#include "variables.h"

Renderer::Renderer(int width, int height){
    nextTextureId = 1;

    geometryBatch.reserve(10000);
    
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
}

void Renderer::drawRect(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;
    float fa = a / 255.0f;
    
    geometryBatch.insert(geometryBatch.end(), {
        (float)x, (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)(y+h), fr, fg, fb, fa,
        
        (float)x, (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)(y+h), fr, fg, fb, fa,
        (float)x, (float)(y+h), fr, fg, fb, fa,
    });

    /*
    float vertices[] = {
        (float)x, (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)(y+h), fr, fg, fb, fa,
        
        (float)x, (float)y, fr, fg, fb, fa,
        (float)(x+w), (float)(y+h), fr, fg, fb, fa,
        (float)x, (float)(y+h), fr, fg, fb, fa,
    };
    
    currentShader->use();
    currentShader->setMat4("projection", projectionMatrix);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);*/
}

void Renderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;
    float fa = a / 255.0f;
    
    geometryBatch.insert(geometryBatch.end(), {
        (float)x1, (float)y1, fr, fg, fb, fa,
        (float)x2, (float)y2, fr, fg, fb, fa,
        (float)x3, (float)y3, fr, fg, fb, fa,
    });

    /*float vertices[] = {
        (float)x1, (float)y1, fr, fg, fb, fa,
        (float)x2, (float)y2, fr, fg, fb, fa,
        (float)x3, (float)y3, fr, fg, fb, fa,
    };
    
    currentShader->use();
    currentShader->setMat4("projection", projectionMatrix);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 3);*/
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

        geometryBatch.insert(geometryBatch.end(), {
            (float)centerX, (float)centerY, fr, fg, fb, fa,
            x1, y1, fr, fg, fb, fa,
            x2, y2, fr, fg, fb, fa
        });
    }
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;
    float fa = a / 255.0f;
    
    float vertices[] = {
        (float)x1, (float)y1, fr, fg, fb, fa,
        (float)x2, (float)y2, fr, fg, fb, fa,
    };
    
    currentShader->use();
    currentShader->setMat4("projection", projectionMatrix);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);
}

int Renderer::loadTexture(const std::string& path){
    auto texture = std::make_shared<Texture>();

    auto loaded = texture->loadFromFile(path,renderer);
    if (!loaded){
        std::cerr << "Failed to load texture";
        return -1;
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

    textureShader->use();
    textureShader->setMat4("projection", projectionMatrix);
    textureShader->setFloat("alpha", texture->getTransparency());
    textureShader->setInt("textureSampler", 0);

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
    return id;
}

void Renderer::drawText(int fontId, const std::string& text, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    auto it = fonts.find(fontId);
    if (it == fonts.end()) return;

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

    float currentX = x;

    for (char c : text){
        Character* ch = font->getCharacter(c);
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

void Renderer::flush(){
    if (geometryBatch.empty()){
        return;
    }

    currentShader->use();
    currentShader->setMat4("projection", projectionMatrix);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, geometryBatch.size()*sizeof(float), geometryBatch.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, geometryBatch.size() / 6);
    geometryBatch.clear();
}