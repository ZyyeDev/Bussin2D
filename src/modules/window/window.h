#pragma once

#include <SDL2/SDL.h>
#include <string>

#include "../graphics/glad.h"

class Window{
public:
    Window();
    ~Window();

    bool init(int width, int height, const std::string& title);
    void clear(unsigned char r, unsigned char g, unsigned char b);
    void present();
    void pollEvents();
    void quit() {isRunning = false;}

    int getWidth() { return width; }
    int getHeight() { return height; }

    SDL_GLContext getGLContext() { return glContext; }
    
    int get_physics_framerate() { return physics_framerate; }
    void set_physics_framerate(int new_physics) { physics_framerate = new_physics; }

    bool isRunning;

    int currentFPS;
    void setVsync(bool enabled);

    void setTargetFPS(int fps) { targetFPS = fps; }
    int getTargetFPS() { return targetFPS; }
private:
    SDL_GLContext glContext;
    SDL_Window* window;
    int width, height;
    int physics_framerate = 60.0;
    int targetFPS = 60;
};