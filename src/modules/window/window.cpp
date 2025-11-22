#include "window.h"
#include "core/common.h"

Window::Window() {
    window = nullptr;
    renderer = nullptr;
    isRunning = false;
}

Window::~Window(){
    if (renderer){
        SDL_DestroyRenderer(renderer);
    }
    if (window){
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool Window::init(int width, int height, const std::string& title){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "STL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window){
        std::cerr << "window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if (!renderer){
        std::cerr << "renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    isRunning = true;
    return true;
}

void Window::clear(unsigned char r, unsigned char g, unsigned char b){
    SDL_SetRenderDrawColor(renderer, r,g,b,255);
    SDL_RenderClear(renderer);
}

void Window::present(){
    SDL_RenderPresent(renderer);
}

void Window::pollEvents(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){
            isRunning = false;
        }
    }
}