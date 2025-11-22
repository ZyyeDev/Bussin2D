#include "keyboard.h"

Keyboard::Keyboard(){
    currentState = SDL_GetKeyboardState(nullptr);
}

Keyboard::~Keyboard(){

}

void Keyboard::handleEvent(const SDL_Event& event){
    if (event.type == SDL_KEYDOWN && !event.key.repeat){
        pressedThisFrame.insert(event.key.keysym.scancode);
    }
    if (event.type == SDL_KEYUP){
        releasedThisFrame.insert(event.key.keysym.scancode);
    }
}

bool Keyboard::isDown(SDL_Scancode key){
    return currentState[key];
}

bool Keyboard::isPressed(SDL_Scancode key){
    return pressedThisFrame.count(key) > 0;
}

bool Keyboard::isReleased(SDL_Scancode key){
    return releasedThisFrame.count(key) > 0;
}

SDL_Scancode Keyboard::getScancodeFromName(const char* name){
    return SDL_GetScancodeFromName(name);
}