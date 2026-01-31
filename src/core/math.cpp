#include <math.h>
#include "math.h"

float flerp(float a, float b, float t){
    return a + (b - a) * t;
}