#include "Effects.h"
#include "EffectUtils.h"
#include "Canvas.h"

int SimpleColumns(Canvas *c, char mode)
{
    static unsigned char n = 0;
    static char currentMode = -1;
    if (mode != currentMode)
    {
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
    } else {
        // 0b00011111 = 0x1F
        n = (n+1) & 0x1F;
    }

    for (char y = 0; y < CANVAS_HEIGHT; y++)
    {
        // Create ascending values
        // [0, 
        
        unsigned char nn = (n+y) & 0x1F;
        /*
        uint32_t color = COLOR(0,nn,0);
        c->PutPixel(0, y, color);
        c->PutPixel(2, y, color);
        c->PutPixel(3, y, color);
        c->PutPixel(5, y, color);
        c->PutPixel(6, y, color);
        c->PutPixel(8, y, color);
        */
        /*
        Color_t color = COLOR_B(31,31,31);
        c->PutPixel(0, y, color);
        c->PutPixel(1, y, color);
        c->PutPixel(2, y, color);
        c->PutPixel(3, y, color);
        c->PutPixel(4, y, color);
        c->PutPixel(5, y, color);
        */

        
        c->PutPixel(0, y, COLOR(nn,0,0));
        c->PutPixel(1, y, COLOR(0,nn,0));

        c->PutPixel(2, y, COLOR(0,0,nn));


        c->PutPixel(3, y, COLOR_B(nn,0,0));

        c->PutPixel(4, y, COLOR_B(0,nn,0));
        c->PutPixel(5, y, COLOR_B(0, 0,nn));
        
    }
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
    
    uint8_t r, g, b;
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            r = max_f(0.0f, 4.0f - dist(x, y, rlx, rly)) * 0x1F;
            g = max_f(0.0f, 3.0f - dist(x, y, glx, gly)) * 0x1F;
            b = max_f(0.0f, 4.0f - dist(x, y, blx, bly)) * 0x1F;
            c->PutPixel(x, y, COLOR_B(min_ub(r, 0x1F), min_ub(g, 0x1f), min_ub(b, 0x1f)));
        }
    }
    
    step++;
}

int Pinwheel(Canvas *c, char mode)
{
    static int step = 0;
    
    
}