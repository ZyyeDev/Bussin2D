#pragma once

#include <SDL2/SDL.h>
#include <unordered_set>

class Keyboard{
public:
    Keyboard();
    ~Keyboard();

    void update();
    void handleEvent(const SDL_Event& event);

    bool isDown(SDL_Scancode key);
    bool isPressed(SDL_Scancode key);
    bool isReleased(SDL_Scancode key);

    static SDL_Scancode getScancodeFromName(const char* name);
private:
    const Uint8* currentState;
    std::unordered_set<SDL_Scancode> pressedThisFrame;
    std::unordered_set<SDL_Scancode> releasedThisFrame;
};