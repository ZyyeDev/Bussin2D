#include "camera.h"
#include "core/common.h"

Camera::Camera(int screenWidth, int screenHeight)
    : x(0), y(0), zoom(1.0f), screenWidth(screenWidth), screenHeight(screenHeight) {}

void Camera::setPosition(float nx, float ny){
    //std::cout << "set pos " << nx << " " << ny << std::endl;
    x = nx;
    y = ny;
}

void Camera::move(float dx, float dy){
    x += dx;
    y += dy;
}

void Camera::setZoom(float z){
    zoom = z;
    if (zoom < 0.1f){
        zoom = 0.1f;
    }
}

void Camera::worldToScreen(float worldX, float worldY, int& screenX, int& screenY){
    screenX = (worldX - x) * zoom;
    screenY = (worldY - y) * zoom;
}
void Camera::screenToWorld(int screenX, int screenY, float& worldX, float& worldY){
    worldX = screenX / zoom + x;
    worldY = screenY / zoom + y;
}