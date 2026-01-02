#include "collision.h"
#include <cmath>

bool Collision::AABB(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
    return x1 < (x2 + w2) &&
           (x1 + w1) > x2 &&
           y1 < (y2 + h2) &&
           (y1 + h1) > y2;
}

bool Collision::Circle(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrt(dx*dx + dy*dy);
    return distance < (r1 + r2);
}

bool Collision::PointInRect(float px, float py, float rx, float ry, float rw, float rh) {
    return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

bool Collision::PointInCircle(float px, float py, float cx, float cy, float r) {
    float dx = px - cx;
    float dy = py - cy;
    return (dx*dx + dy*dy) < (r*r);
}