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
    void setSize(int w, int h);
    void onResize(int w, int h);

    void setFullscreen(bool fullscreen);
    void setBorderless(bool borderless);
    bool isFullscreen();
    bool isBorderless();

    void setTitle(const std::string& title);
    std::string getTitle();

    void setPosition(int x, int y);
    void getPosition(int* x, int* y);

    void maximize();
    void minimize();
    void restore();
    bool hasFocus();
    bool isMaximized();
    bool isMinimized();

    void setResizable(bool resizable);
    bool isResizable();

    SDL_GLContext getGLContext() { return glContext; }
    SDL_Window* getSDLWindow() { return window; }

    int get_physics_framerate() { return physics_framerate; }
    void set_physics_framerate(int fps) { physics_framerate = fps; }

    bool isRunning;
    int currentFPS;

    void setVsync(bool enabled);
    void setTargetFPS(int fps) { targetFPS = fps; }
    int getTargetFPS() { return targetFPS; }
private:
    SDL_GLContext glContext;
    SDL_Window* window;
    int width, height;
    int physics_framerate = 60;
    int targetFPS = 60;
};