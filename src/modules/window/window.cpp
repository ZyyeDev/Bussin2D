#include "window.h"
#include "core/common.h"

#include "variables.h"

Window::Window() {
    window = nullptr;
    isRunning = false;
}

Window::~Window(){
    if (glContext){
        SDL_GL_DeleteContext(glContext);
    }
    if (window){
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool Window::init(int w, int h, const std::string& title){
    width = w;
    height = h;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "STL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    if (!window){
        std::cerr << "window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext){
        std::cerr << "OpenGL error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // vsync
    SDL_GL_SetSwapInterval(1);

    isRunning = true;
    return true;
}

void Window::clear(unsigned char r, unsigned char g, unsigned char b){
    glClearColor(r/255.0f, g/255.0f, b/255.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::present(){
    SDL_GL_SwapWindow(window);
    if (!g_renderer) init_renderer();
    if (g_renderer){
        g_renderer->createProjectionMatrix(width, height);
    }
}

void Window::pollEvents(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){
            isRunning = false;
        }
    }
}

void Window::setVsync(bool enabled){
    int interval = 0;
    if (enabled) interval = 1;
    if (SDL_GL_SetSwapInterval(interval) < 0){
        std::cerr << Colors::RED << "Error: Unable to set vsync! Error: " << SDL_GetError() << std::endl;
        if (interval == -1) SDL_GL_SetSwapInterval(1);
    }
}