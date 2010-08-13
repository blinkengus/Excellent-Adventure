#define MOD32(C) ((C) & 0x1F)

#include <math.h>


typedef unsigned char uint8_t;


static uint8_t sin_lut[] =
{
    127, 152, 176, 198, 217, 233, 245, 252, 255, 252, 245, 233, 217, 198, 176, 152, 127, 102, 78, 56, 37, 21, 9, 2, 0, 2, 9, 21, 37, 56, 78, 102
};

static Color_t colorwheel_lut[] =
{
    0x801f, 0x80bf, 0x817f, 0x823f, 0x82ff, 0x83bf, 0x83fb, 0x83f5, 0x83ef, 0x83e9, 0x83e3, 0x87e0, 0x9fe0, 0xb7e0, 0xcfe0, 0xe7e0, 0xffe0, 0xff20, 0xfe60, 0xfda0, 0xfce0, 0xfc20, 0xfc03, 0xfc09, 0xfc0f, 0xfc15, 0xfc1b, 0xf41f, 0xdc1f, 0xc41f, 0xac1f, 0x941f
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

inline uint8_t min_ub(uint8_t v1, uint8_t v2)
{
    return v1 < v2 ? v1 : v2;
}