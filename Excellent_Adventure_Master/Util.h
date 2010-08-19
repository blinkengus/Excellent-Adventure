#define MOD32(C) ((C) & 0x1F)


#include <math.h>


typedef unsigned char ubyte;


static ubyte sin_lut[] =
{
    127, 152, 176, 198, 217, 233, 245, 252, 255, 252, 245, 233, 217, 198, 176, 152, 127, 102, 78, 56, 37, 21, 9, 2, 0, 2, 9, 21, 37, 56, 78, 102
};


float dist(float x1, float y1, float x2, float y2)
{
    float xo = (x2 - x1) * 1.666666f;
    float yo = y2 - y1;
    return sqrt(xo * xo + yo * yo);
}


inline float max_f(float v1, float v2)
{
    return v1 > v2 ? v1 : v2;
}

inline ubyte min_ub(ubyte v1, ubyte v2)
{
    return v1 < v2 ? v1 : v2;
}