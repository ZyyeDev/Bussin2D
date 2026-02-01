#include <math.h>
#include "math.h"

float flerp(float a, float b, float t){
    return a + (b - a) * t;
}

float point_distance(float x1, float y1, float x2, float y2){
    return std::sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2));
}