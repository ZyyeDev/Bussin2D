#pragma once

#include <cmath>

struct Vector2{
    float x, y;

    Vector2() : x(0) , y(0) {};
    Vector2(float x, float y) : x(x), y(y) {};

    Vector2 operator+(const Vector2& other) const{
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const{
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const{
        return Vector2(x * scalar, y * scalar);
    }

    float length() const {
        return std::sqrt((x*x) + (y*y));
    }

    Vector2 normalized() const {
        float len = length();
        if (len > 0) return Vector2(x / len, y / len);
        return Vector2(0.0f,0.0f);
    }
};

struct Rect{
    float x, y, w, h;

    Rect() : x(0), y(0), w(0), h(0) {};
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {};
};

float flerp(float a, float b, float t);