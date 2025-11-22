#pragma once

#include <SDL2/SDL.h>
#include <unordered_set>

class Mouse{
public:
    Mouse();
    ~Mouse();

    void update();
    void handleEvent(const SDL_Event& event);

    int getX();
    int getY();
    bool isDown(int button);
    bool isPressed(int button);
    bool isReleased(int button);
    int getScrollY();
private:
    int x, y;
    int scrollY;
    Uint32 buttonState;
    std::unordered_set<int> pressedThisFrame;
    std::unordered_set<int> releasedThisFrame;
};