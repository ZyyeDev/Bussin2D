#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "color.h"

HSV RGBtoHSV(float fR, float fG, float fB) {
    float fCMax = std::max(std::max(fR, fG), fB);
    float fCMin = std::min(std::min(fR, fG), fB);
    float fDelta = fCMax - fCMin;
  
    HSV oHSV;
  
    if(fDelta > 0) {
      if(fCMax == fR) {
        oHSV.H = 60 * (fmod(((fG - fB) / fDelta), 6));
      } else if(fCMax == fG) {
        oHSV.H = 60 * (((fB - fR) / fDelta) + 2);
      } else if(fCMax == fB) {
        oHSV.H = 60 * (((fR - fG) / fDelta) + 4);
      }
      
      if(fCMax > 0) {
        oHSV.S = fDelta / fCMax;
      } else {
        oHSV.S = 0;
      }
      
      oHSV.V = fCMax;
    } else {
      oHSV.H = 0;
      oHSV.S = 0;
      oHSV.V = fCMax;
    }
    
    if(oHSV.H < 0) {
      oHSV.H = 360 + oHSV.H;
    }
    return oHSV;
}

RGB HSVtoRGB(float fH, float fS, float fV){
    RGB oRGB;
    float fC = fV * fS; // Chroma
    float fHPrime = fmod(fH / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;

    if(0 <= fHPrime && fHPrime < 1) {
      oRGB.R = fC;
      oRGB.G = fX;
      oRGB.B = 0;
    } else if(1 <= fHPrime && fHPrime < 2) {
      oRGB.R = fX;
      oRGB.G = fC;
      oRGB.B = 0;
    } else if(2 <= fHPrime && fHPrime < 3) {
      oRGB.R = 0;
      oRGB.G = fC;
      oRGB.B = fX;
    } else if(3 <= fHPrime && fHPrime < 4) {
      oRGB.R = 0;
      oRGB.G = fX;
      oRGB.B = fC;
    } else if(4 <= fHPrime && fHPrime < 5) {
      oRGB.R = fX;
      oRGB.G = 0;
      oRGB.B = fC;
    } else if(5 <= fHPrime && fHPrime < 6) {
      oRGB.R = fC;
      oRGB.G = 0;
      oRGB.B = fX;
    } else {
      oRGB.R = 0;
      oRGB.G = 0;
      oRGB.B = 0;
    }
  
    oRGB.R += fM;
    oRGB.G += fM;
    oRGB.B += fM;

    return oRGB;
}
