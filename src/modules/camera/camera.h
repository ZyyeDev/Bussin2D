#pragma once

class Camera{
public:
    Camera(int screenWidth, int screenHeight);

    void setPosition(float x, float y);
    void move(float dx, float dy);
    void setZoom(float z);

    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY);
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY);

    float getX() const {return x;}
    float getY() const {return y;}
    float getZoom() const {return zoom;}
private:
    float x, y;
    float zoom;
    int screenWidth, screenHeight;
};