#include "mouse.h"

Mouse::Mouse(){
    x = 0;
    y = 0;
    scrollY = 0;
    buttonState = 0;
};

Mouse::~Mouse(){

};

void Mouse::update(){
    pressedThisFrame.clear();
    releasedThisFrame.clear();
    scrollY = 0;
    buttonState = SDL_GetMouseState(&x, &y);
}

void Mouse::handleEvent(const SDL_Event& event){
    if (event.type == SDL_MOUSEBUTTONDOWN){
        pressedThisFrame.insert(event.button.button);
    }
    if (event.type == SDL_MOUSEBUTTONUP){
        releasedThisFrame.insert(event.button.button);
    }
    if (event.type == SDL_MOUSEWHEEL){
        scrollY = event.wheel.y;
    }
}

int Mouse::getX(){
    return x;
}

int Mouse::getY(){
    return y;
}

bool Mouse::isDown(int button){
    return buttonState & SDL_BUTTON(button);
}
bool Mouse::isPressed(int button){
    return pressedThisFrame.count(button) > 0;
}
bool Mouse::isReleased(int button){
    return releasedThisFrame.count(button) > 0;
}

int Mouse::getScrollY(){
    return scrollY;
}