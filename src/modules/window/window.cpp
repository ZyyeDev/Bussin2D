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
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
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
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
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

    glViewport(0, 0, width, height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // vsync
    SDL_GL_SetSwapInterval(1);

    isRunning = true;
    return true;
}

void Window::clear(unsigned char r, unsigned char g, unsigned char b){
    if (g_renderer && g_renderer->hasVirtualResolution()){
        g_renderer->beginVirtualFrame();
    }
    glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::present(){
    // determine the resolution the projection matrix should match.
    // with virtual resolution the game always "thinks" the canvas is
    // virtualWidth × virtualHeight, so coordinates stay consistent
    // regardless of the real window size
    if (g_renderer){
        if (g_renderer->hasVirtualResolution()){
            g_renderer->createProjectionMatrix(
                g_renderer->getVirtualWidth(),
                g_renderer->getVirtualHeight()
            );
            g_renderer->endVirtualFrame(width, height);
        } else {
            g_renderer->createProjectionMatrix(width, height);
        }
    }

    SDL_GL_SwapWindow(window);

    if (!g_renderer) init_renderer();
}

void Window::onResize(int w, int h){
    width  = w;
    height = h;
    
    if (!g_renderer || !g_renderer->hasVirtualResolution()){
        glViewport(0, 0, w, h);
    }
}

void Window::setSize(int w, int h){
    if (!window) return;
    SDL_SetWindowSize(window, w, h);
    onResize(w, h);
}

void Window::setFullscreen(bool fullscreen){
    if (!window) return;
    Uint32 flags = 0;
    if (fullscreen){
        flags = SDL_WINDOW_FULLSCREEN;
    }
    SDL_SetWindowFullscreen(window, flags);
    
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    onResize(w, h);
}

void Window::setBorderless(bool borderless){
    if (!window) return;
    Uint32 flags = 0;
    if (borderless){
        flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    SDL_SetWindowFullscreen(window, flags);
    
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    onResize(w, h);
}

bool Window::isFullscreen(){
    if (!window) return false;
    Uint32 flags = SDL_GetWindowFlags(window);
    return (flags & SDL_WINDOW_FULLSCREEN) != 0;
}

bool Window::isBorderless(){
    if (!window) return false;
    Uint32 flags = SDL_GetWindowFlags(window);
    return (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
}

void Window::setTitle(const std::string& title){
    if (window) SDL_SetWindowTitle(window, title.c_str());
}

std::string Window::getTitle(){
    if (!window) return "";
    return std::string(SDL_GetWindowTitle(window));
}

void Window::setPosition(int x, int y){
    if (window) SDL_SetWindowPosition(window, x, y);
}

void Window::getPosition(int* x, int* y){
    if (window) SDL_GetWindowPosition(window, x, y);
}

void Window::maximize(){
    if (window) SDL_MaximizeWindow(window);
}

void Window::minimize(){
    if (window) SDL_MinimizeWindow(window);
}

void Window::restore(){
    if (window) SDL_RestoreWindow(window);
}

bool Window::hasFocus(){
    if (!window) return false;
    return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

bool Window::isMaximized(){
    if (!window) return false;
    return (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
}

bool Window::isMinimized(){
    if (!window) return false;
    return (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) != 0;
}

void Window::setResizable(bool resizable){
    if (window){
        if (resizable){
            SDL_SetWindowResizable(window, SDL_TRUE);
        }else{
            SDL_SetWindowResizable(window, SDL_FALSE);
        }
    }
}

bool Window::isResizable(){
    if (!window) return false;
    return (SDL_GetWindowFlags(window) & SDL_WINDOW_RESIZABLE) != 0;
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
        std::cerr << Colors::RED
                  << "Error: Unable to set vsync: " << SDL_GetError()
                  << std::endl;
        if (interval == -1) SDL_GL_SetSwapInterval(1);
    }
}