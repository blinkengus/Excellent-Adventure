#include "EffectManager.h"
#include "Effects.h"
#include "EffectUtils.h"
#include "Canvas.h"
#include "Spectrum.h"


int SimpleSpectrum(Canvas *c, EffectManager *em, char mode)
{
    static unsigned n = 0;
    static char currentMode = -1;
    if (mode != currentMode)
    {
        // Performed only once.
        switch(mode)
        {
        case EFFECTMODE_INTRO:
            break;
        case EFFECTMODE_LOOP:
            break;
        case EFFECTMODE_OUTRO:
            break;
        }
        currentMode = mode;
    } else {
        // 0b00011111 = 0x1F
        n++;// = (n+1) & 0x1F;
    }
    
    unsigned short *bands = em->GetSpectrum();

    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
    {
        unsigned high;
        Color_t color;
        switch (y)
        {
        case 0:     // Yellow
            color = COLOR_B(31,31,0);
            high = 900;
            break;
        case 1:
            color = COLOR_B(23,23,0);
            high = 800;
            break;
        case 2:
            color = COLOR(31,31,0);
            high = 700;
            break;
        case 3:     // Green
            color = COLOR_B(0,31,0);
            high = 600;
            break;
        case 4:
            color = COLOR_B(0,23,0);
            high = 500;
            break;
        case 5:
            color = COLOR(0,31,0);
            high = 400;
            break;
        case 6:     // Red
            color = COLOR_B(31,0,0);
            high = 300;
            break;
        case 7:
            color = COLOR_B(23,0,0);
            high = 200;
            break;
        case 8:
            color = COLOR(31,0,0);
            high = 100;
            break;
        case 9:
            color = COLOR(16,0,0);
            high = 0;
            break;
        default: 
            color = 0;
        }

        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
        {
            c->PutPixel(x,y, (bands[x] > high) ? color : 0);
        }
    }
    return 1;
}


int WarpSpectrum(Canvas *c, EffectManager *em, char mode)
{
    static unsigned short pos[CANVAS_WIDTH];
    static          short vel[CANVAS_WIDTH];
    
    static char currentMode = -1;
    if(mode != currentMode){  // performed only once
        switch(mode){
            case EFFECTMODE_INTRO:
                for(uint8_t x = 0; x < CANVAS_WIDTH; x++){
                    pos[x] = CANVAS_HM1;
                    // for(uint8_t y = 0; y < CANVAS_HEIGHT; y++)
                    //     c->PutPixel(x, y, 0);
                }
                break;
        }
        currentMode = mode;
    }
    else{  // step
        unsigned short *spectrum = em->GetSpectrum();
        for(uint8_t x = 0; x < CANVAS_WIDTH; x++){
            vel[x] += ((short)spectrum[x] - 150) * 10;
        }
    }

    for(uint8_t x = 0; x < CANVAS_WIDTH; x++){
        uint8_t px = pos[x] / 6554;
        for(uint8_t y = 0; y < CANVAS_HEIGHT; y++){
            c->PutPixel(x, y, y == px ? COLOR_B(0x1f,0,0) : COLOR_B(0,0,0));
        }
    }
    
    return 1;
}


int SimpleColumns(Canvas *c, EffectManager *em, char mode)
{
    static Channel_t n = 0;
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


    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
    {
        // Create ascending values
        // [0, 
        
        Channel_t nn = (n+y) & 0x1F;
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
    return 1;
}

int Spotlight(Canvas *c, EffectManager *em, char mode)
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
    
    return 1;
}

int CheckerBoard(Canvas *c, EffectManager *em, char mode)
{
    // Sub-pixel
    static unsigned char offsetX = 0;

    static unsigned char offsetY = 0;

    static Channel_t n = 0;
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

    Color_t color = COLOR(n,n,n);

    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
    {
        char offset = offsetX+offsetY+y;
        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
        {
            bool draw = ((x+offset) & 0x1) == 0;
            if (draw)
            {
                c->PutPixel(x,y, color);
            } else {
                c->PutPixel(x,y, 0);
            }
        }
        
    }
    if (n == 0)
    {
        offsetX++;
        //if ((offsetX & 3) == 3)
        //{
        //    offsetY++;
        //}
    }
    return 1;
}
