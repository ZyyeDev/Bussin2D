#pragma once

class Collision{
public:
    static bool AABB(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
    static bool Circle(float x1, float y1, float r1, float x2, float y2, float r2);
    static bool PointInRect(float px, float py, float rx, float ry, float rw, float rh);
    static bool PointInCircle(float px, float py, float cx, float cy, float r);
};