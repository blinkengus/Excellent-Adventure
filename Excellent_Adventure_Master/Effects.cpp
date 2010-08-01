#include "Effects.h"
#include "Canvas.h"
#include "Util.h"

int Spotlight(Canvas *c, char mode)
{
    static int step = 0;
    
    float rlx = sin_lut[MOD32(step + 8)] / 255.0f * CANVAS_WM1;
    float rly = sin_lut[MOD32(step)] / 255.0f * CANVAS_HM1;
    
    float glx = sin_lut[MOD32(-step + 8)] / 255.0f * CANVAS_WM1;
    float gly = sin_lut[MOD32(-step)] / 255.0f * CANVAS_HM1;
    
    float blx = sin_lut[MOD32(step + 24)] / 255.0f * CANVAS_WM1;
    float bly = sin_lut[MOD32(step + 16)] / 255.0f * CANVAS_HM1;
    
    ubyte r, g, b;
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