#pragma once

#include <SDL2/SDL.h>
#include <string>

class Window{
public:
    Window(),
    ~Window();

    bool init(int width, int height, const std::string& title);
    void clear(unsigned char r, unsigned char g, unsigned char b);
    void present();
    void pollEvents();
    void quit() {isRunning = false;}

    SDL_Renderer* getRenderer() { return renderer; }
    bool isRunning;
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};