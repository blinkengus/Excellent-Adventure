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
    //static int sin_speed_step = 0;
    
    static int ctrx = (CANVAS_WM1 * 100) / 2;
    static int ctry = (CANVAS_HM1 * 100) / 2;
    
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            int angle = MOD32(int(atan2(y * 100 - ctry, x * 100 - ctrx) * TWO_PI_TO_32) + step);
            //int bri = sin_lut[MOD32(dist(x, y, rlx, rly) + step);
            c->PutPixel(x, y, colorwheel_lut[angle]);// * bri);
        }
    }
    
    step += 2;
}

int MatrixRain(Canvas *c, char mode)
{
    static uint8_t pos[] = { CANVAS_HEIGHT, CANVAS_HEIGHT, CANVAS_HEIGHT, CANVAS_HEIGHT, CANVAS_HEIGHT, CANVAS_HEIGHT };
    static uint8_t vel[] = { 0, 0, 0, 0, 0, 0 };
    
    for(char x = 0; x < 6; x++){
        pos[x] += vel[x];
        if(pos[x] >= CANVAS_HEIGHT){
            pos[x] = 0;
            vel[x] = x % 3;
        }
        c->PutPixel(x, pos[x], COLOR_B(0, 31, 0));
    }
    
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            Color_t px = c->GetPixel(x, y);
            c->PutPixel(x, y, COLOR_B(max_ub(0, RED(px) - 2), max_ub(0, GREEN(px) - 2), max_ub(0, BLUE(px) - 2)));
        }
    }
}


/*
Ideas
- Matrix Mode (Horiz + Vertical)
- More Single Color Stuff
- Strobing
- Overhead Flicker
*/