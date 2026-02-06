#pragma once

struct HSV {
    float H,S,V;
};

struct RGB {
    float R,G,B;
};

struct RGBA {
    float R,G,B,A;
};

HSV RGBtoHSV(float fR, float fG, float fB);
RGB HSVtoRGB(float fH, float fS, float fV);
