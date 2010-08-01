#include "Effects.h"
#include "Canvas.h"

#include <math.h>

int SimpleColumns(Canvas *c, char mode)
{
    static unsigned char n = 0;
    static char currentMode = -1;
    
    if(mode != currentMode){
        // Performed only once.
        switch(mode)
        {
        case EFFECTMODE_INTRO:
            n = 0;
            break;
        case EFFECTMODE_LOOP:
            break;
        case EFFECTMODE_OUTRO:
            break;
        }
        currentMode = mode;
    }
    else{
        // 0b00011111 = 0x1F
        n = (n+1) & 0x1F;
    }

    for(char y = 0; y < CANVAS_HEIGHT; y++){
        unsigned char nn = (n+y) & 0x1F;
        
        c->PutPixel(0, y, COLOR(nn,0,0));
        c->PutPixel(1, y, COLOR(0,nn,0));

        c->PutPixel(2, y, COLOR(0,0,nn));


        c->PutPixel(3, y, COLOR_B(nn,0,0));

        c->PutPixel(4, y, COLOR_B(0,nn,0));
        c->PutPixel(5, y, COLOR_B(0, 0,nn));
    }
}

float dist(float x1, float y1, float x2, float y2)
{
    float xo = (x2 - x1) * 1.666666f;
    float yo = y2 - y1;
    return sqrt(xo * xo + yo * yo);
}
inline float max(float v1, float v2)
{
    return v1 > v2 ? v1 : v2;
}
inline unsigned char min(unsigned char v1, unsigned char v2)
{
    return v1 < v2 ? v1 : v2;
}

int Spotlight(Canvas *c, char mode)
{
    static int step = 0;
    
    float rlx = sin_lut[MOD32(step + 8)] / 255.0f * CANVAS_WM1;
    float rly = sin_lut[MOD32(step)] / 255.0f * CANVAS_HM1;
    
    float glx = sin_lut[MOD32(-step + 8)] / 255.0f * CANVAS_WM1;
    float gly = sin_lut[MOD32(-step)] / 255.0f * CANVAS_HM1;
    
    float blx = sin_lut[MOD32(step + 24)] / 255.0f * CANVAS_WM1;
    float bly = sin_lut[MOD32(step + 16)] / 255.0f * CANVAS_HM1;
    
    unsigned char r, g, b;
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            r = max(0.0f, 4.0f - dist(x, y, rlx, rly)) * 0x1F;
            g = max(0.0f, 3.0f - dist(x, y, glx, gly)) * 0x1F;
            b = max(0.0f, 4.0f - dist(x, y, blx, bly)) * 0x1F;
            
            c->PutPixel(x, y, COLOR_B(min(r, 0x1F), min(g, 0x1f), min(b, 0x1f)));
        }
    }
    
    step++;
}