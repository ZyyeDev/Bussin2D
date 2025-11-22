#pragma once

struct Vector2{
    float x, y;

    Vector2() : x(0) , y(0) {};
    Vector2(float x, float y) : x(x), y(y) {};
};

struct Color{
    unsigned char r, g, b, a;

    Color(): r(255), g(255), b(255), a(255) {};
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : r(r), g(g), b(b), a(a) {};
};

struct Rect{
    float x, y, w, h;

    Rect() : x(0), y(0), w(0), h(0) {};
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {};
};
