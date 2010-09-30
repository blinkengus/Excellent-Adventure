#include "EffectManager.h"
#include "Effects.h"
#include "EffectUtils.h"
#include "Canvas.h"
#include "Spectrum.h"

#ifndef MAX(a,b)
#define MAX(a,b) (a) > (b) ? (a) : (b)
#endif

#ifndef MIN(a,b)
#define MIN(a,b) (a) < (b) ? (a) : (b)
#endif

#define MAX31(V)  MAX((V),31)


//int SimpleSpectrum(Canvas *c, EffectManager *em, char mode)
//{
//    static unsigned n = 0;
//    static char currentMode = -1;
//    if (mode != currentMode)
//    {
//        // Performed only once.
//        switch(mode)
//        {
//        case EFFECTMODE_INTRO:
//            break;
//        case EFFECTMODE_LOOP:
//            break;
//        case EFFECTMODE_OUTRO:
//            break;
//        }
//        currentMode = mode;
//    } else {
//        // 0b00011111 = 0x1F
//        n++;// = (n+1) & 0x1F;
//    }
//    
//    unsigned short *bands = em->GetSpectrum();
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        unsigned high;
//        Color_t color;
//        switch (y)
//        {
//        case 0:     // Yellow
//            color = COLOR_B(31,31,0);
//            high = 900;
//            break;
//        case 1:
//            color = COLOR_B(23,23,0);
//            high = 800;
//            break;
//        case 2:
//            color = COLOR(31,31,0);
//            high = 700;
//            break;
//        case 3:     // Green
//            color = COLOR_B(0,31,0);
//            high = 600;
//            break;
//        case 4:
//            color = COLOR_B(0,23,0);
//            high = 500;
//            break;
//        case 5:
//            color = COLOR(0,31,0);
//            high = 400;
//            break;
//        case 6:     // Red
//            color = COLOR_B(31,0,0);
//            high = 300;
//            break;
//        case 7:
//            color = COLOR_B(23,0,0);
//            high = 200;
//            break;
//        case 8:
//            color = COLOR(31,0,0);
//            high = 100;
//            break;
//        case 9:
//            color = COLOR(16,0,0);
//            high = 0;
//            break;
//        default: 
//            color = 0;
//        }
//
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            c->PutPixel(x, y, (bands[x + 1] > high) ? color : 0);
//        }
//    }
//    return 1;
//}

int WarpSpectrum(Canvas *c, EffectManager *em, char mode)
{
    static uint8_t ci = 0;
    static unsigned short pos[CANVAS_WIDTH];
    
    static char currentMode = -1;
    if(mode != currentMode){  // performed only once
        switch(mode){
            case EFFECTMODE_INTRO:
                for(uint8_t x = 0; x < CANVAS_WIDTH; x++){
                    pos[x] = CANVAS_HM1;
                    for(uint8_t y = 0; y < CANVAS_HEIGHT; y++)
                        c->PutPixel(x, y, 0);
                }
                break;
        }
        currentMode = mode;
    }
    else{  // step
        unsigned short *spectrum = em->GetSpectrum();
        for(uint8_t x = 0; x < CANVAS_WIDTH; x++){
            short spc = spectrum[(x >> 1) + 3];
            if(spc > 800)
                ci = MOD32(ci + 1);
            pos[x] -= (spc - 150) * 10;
        }
    }

    for(uint8_t x = 0; x < CANVAS_WIDTH; x++){
        uint8_t px = pos[x] / 6554;
        for(uint8_t y = 0; y < CANVAS_HEIGHT; y++){
            if(y == px){  // shot of color
                c->PutPixel(x, y, lerpColor(c->GetPixel(x, y), colorwheel_lut[ci], 0.5));
            }
            else if(y == px - 1 || y == px + 1){  // shot of color
                c->PutPixel(x, y, lerpColor(c->GetPixel(x, y), colorwheel_lut[ci], 0.1));
            }
            else{  // fade out
                Color_t px_color = c->GetPixel(x, y);
                c->PutPixel(x, y, COLOR_B(
                    MAX(0, char(RED(px_color)) - 2),
                    MAX(0, char(GREEN(px_color)) - 2),
                    MAX(0, char(BLUE(px_color)) - 2)
                ));
            }
        }
    }
    
    return 1;
}

int ElevatorSpectrum(Canvas *c, EffectManager *em, char mode)
{
    static unsigned short pos = 0;
    static          short vel = 0;
    
    // step
    unsigned short level = em->GetSpectrum()[4];
    vel *= 0.9;
    vel += level * 2;
    pos -= vel;

    for(uint8_t y = 0; y < CANVAS_HEIGHT; y++){
        Color_t color;
        if(vel > 9000){
            uint8_t cw_pos = (pos + (y << 12)) >> 11;
            color = colorwheel_lut[cw_pos];
        }
        else{
            uint8_t sin_pos = (pos + (y << 13)) >> 11;  // 65536 -> 32
            uint8_t gs = (uint8_t)((sin_lut[sin_pos] * (MIN(512, level) >> 1)) >> 11);
            color = COLOR_B(gs, gs, gs);
        }
        for(uint8_t x = 0; x < CANVAS_WIDTH; x++){
            c->PutPixel(x, y, color);
        }
    }
    
    return 1;
}

#define CTRX ((CANVAS_WM1 * 100) / 2)
#define CTRY ((CANVAS_HM1 * 100) / 2)
int PinwheelSpectrum(Canvas *c, EffectManager *em, char mode)
{
    static int step = 0;
    
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            int angle = MOD32(int(atan2(y * 100 - CTRY, x * 100 - CTRX) * TWO_PI_TO_32) + step);
            c->PutPixel(x, y, colorwheel_lut[angle]);
        }
    }
    
    unsigned short level = em->GetSpectrum()[4];
    
    step += level >> 7; // 0 - 8
}

int SolidColors(Canvas *c, EffectManager *em, char mode)
{
    static Color_t color, color_goal;
    static uint8_t cw_pos;
    
    static char currentMode = -1;
    if(mode != currentMode){  // performed only once
        switch(mode){
            case EFFECTMODE_INTRO:
                cw_pos = 0;
                color = colorwheel_lut[cw_pos];
                color_goal = color;
                break;
        }
        currentMode = mode;
    }
    
    unsigned short *spectrum = em->GetSpectrum();
    unsigned short spc_max = 0;
    for(char x = 0; x < CANVAS_WIDTH; x++){
        unsigned short spc = spectrum[3 + (x >> 1)];
        spc_max = MAX(spc_max, spc);
        Color_t px_color = lerpColor(0, color, MIN(600, spc) / 600.0f);
        for(char y = 0; y < CANVAS_HEIGHT; y++){
            c->PutPixel(x, y, px_color);
        }
    }
    
    if(spc_max > 800){
        cw_pos = (cw_pos + 1) & 31;
        color_goal = colorwheel_lut[cw_pos];
    }
    color = lerpColor(color, color_goal, 0.1f);

    return 1;
}

int SolidSpectrum(Canvas *c, EffectManager *em, char mode)
{
    static Color_t color, color_goal;
    static uint8_t cw_pos;
    
    static char currentMode = -1;
    if(mode != currentMode){  // performed only once
        switch(mode){
            case EFFECTMODE_INTRO:
                cw_pos = 0;
                color = colorwheel_lut[cw_pos];
                color_goal = color;
                break;
        }
        currentMode = mode;
    }
    
    unsigned short *spectrum = em->GetSpectrum();
    unsigned short spc, spc_max = 0;
    for(char x = 0; x < CANVAS_WIDTH; x++){
        spc = spectrum[3 + (x >> 1)];
        spc_max = MAX(spc_max, spc);
        char spc_y = CANVAS_HEIGHT - (spc / 100 + 1);
        for(char y = 0; y < CANVAS_HEIGHT; y++){
            if(spc_y <= y)
                c->PutPixel(x, y, color);
            else
                c->PutPixel(x, y, 0);
        }
    }
    
    if(spc_max > 800){
        cw_pos = (cw_pos + 1) & 31;
        color_goal = colorwheel_lut[cw_pos];
    }
    color = lerpColor(color, color_goal, 0.1);

    return 1;
}

int Accumulator(Canvas *c, EffectManager *em, char mode)
{
    static Color_t color;
    
    static char currentMode = -1;
    if(mode != currentMode){  // performed only once
        switch(mode){
            case EFFECTMODE_INTRO:
                color = colorwheel_lut[em->GetRandomNumber() & 31];
                c->Clear(color);
                break;
        }
        currentMode = mode;
    }
    
    unsigned short level = em->GetSpectrum()[4];
    if(level > 100){
        for(char i = level >> 5; --i >= 0;)
            c->PutPixel(em->GetRandom(6), em->GetRandom(10), color);
            
        char nset = 0;
        for(char y = 0; y < CANVAS_HEIGHT; y++){
            for(char x = 0; x < CANVAS_WIDTH; x++){
                if(c->GetPixel(x, y) == color)
                    nset++;
            }
        }
        
        if(nset == CANVAS_WIDTH * CANVAS_HEIGHT){
            Color_t color_tmp = colorwheel_lut[em->GetRandomNumber() & 31];
            color = lerpColor(
                COLOR_B(RED(color_tmp) >> 2, GREEN(color_tmp) >> 2, BLUE(color_tmp) >> 2),
                color_tmp,
                em->GetRandom(100) / 100.0f
            );
        }
    }
}

// int SpotlightSpectrum(Canvas *c, EffectManager *em, char mode)
// {
//     static int step = 0;
//     
//     unsigned short rlx = (sin_lut[MOD32(step + 8)] * CANVAS_WM1) << 8;
//     unsigned short rly = (sin_lut[MOD32(step)]     * CANVAS_HM1) << 8;
//     
//     unsigned short glx = (sin_lut[MOD32(-step + 8)] * CANVAS_WM1) << 8;
//     unsigned short gly = (sin_lut[MOD32(-step)]     * CANVAS_HM1) << 8;
//     
//     unsigned short blx = (sin_lut[MOD32(step + 24)] * CANVAS_WM1) << 8;
//     unsigned short bly = (sin_lut[MOD32(step + 16)] * CANVAS_HM1) << 8;
//     
//     uint8_t r, g, b;
//     for(char y = 0; y < CANVAS_HEIGHT; y++){
//         for(char x = 0; x < CANVAS_WIDTH; x++){
//             unsigned short xi = x << 8;  // * 256
//             unsigned short yi = y << 8;  // * 256
//             r = MAX(0, (4 << 8) - distInt(xi, yi, rlx, rly)) >> 3;
//             g = MAX(0, (3 << 8) - distInt(xi, yi, glx, gly)) >> 3;
//             b = MAX(0, (4 << 8) - distInt(xi, yi, blx, bly)) >> 3;
//             c->PutPixel(x, y, COLOR_B(MIN(r, 31), MIN(g, 31), MIN(b, 31)));
//         }
//     }
//     
//     step++;
//     
//     return 1;
// }


//int SimpleColumns(Canvas *c, EffectManager *em, char mode)
//{
//    static Channel_t n = 0;
//    static char currentMode = -1;
//    if (mode != currentMode)
//    {
//        // Performed only once.
//        switch(mode)
//        {
//        case EFFECTMODE_INTRO:
//            n = 0;
//            break;
//        case EFFECTMODE_LOOP:
//            break;
//        case EFFECTMODE_OUTRO:
//            break;
//        }
//        currentMode = mode;
//    } else {
//        // 0b00011111 = 0x1F
//        n = (n+1) & 0x1F;
//    }
//
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        // Create ascending values
//        // [0, 
//        
//        Channel_t nn = (n+y) & 0x1F;
//        /*
//        uint32_t color = COLOR(0,nn,0);
//        c->PutPixel(0, y, color);
//        c->PutPixel(2, y, color);
//        c->PutPixel(3, y, color);
//        c->PutPixel(5, y, color);
//        c->PutPixel(6, y, color);
//        c->PutPixel(8, y, color);
//        */
//        /*
//        Color_t color = COLOR_B(31,31,31);
//        c->PutPixel(0, y, color);
//        c->PutPixel(1, y, color);
//        c->PutPixel(2, y, color);
//        c->PutPixel(3, y, color);
//        c->PutPixel(4, y, color);
//        c->PutPixel(5, y, color);
//        */
//
//        
//        c->PutPixel(0, y, COLOR(nn,0,0));
//        c->PutPixel(1, y, COLOR(0,nn,0));
//
//        c->PutPixel(2, y, COLOR(0,0,nn));
//
//
//        c->PutPixel(3, y, COLOR_B(nn,0,0));
//
//        c->PutPixel(4, y, COLOR_B(0,nn,0));
//        c->PutPixel(5, y, COLOR_B(0, 0,nn));
//        
//    }
//    return 1;
//}

int SpotlightSpectrum(Canvas *c, EffectManager *em, char mode)
{
    static uint8_t rstep = 0;
    static uint8_t gstep = 0;
    static uint8_t bstep = 0;
    
    float rlx = sin_lut[MOD32(rstep + 8)] / 255.0f * CANVAS_WM1;
    float rly = sin_lut[MOD32(rstep)] / 255.0f * CANVAS_HM1;
    
    float glx = sin_lut[MOD32(-gstep + 8)] / 255.0f * CANVAS_WM1;
    float gly = sin_lut[MOD32(-gstep)] / 255.0f * CANVAS_HM1;
    
    float blx = sin_lut[MOD32(bstep + 24)] / 255.0f * CANVAS_WM1;
    float bly = sin_lut[MOD32(bstep + 16)] / 255.0f * CANVAS_HM1;
    
    unsigned short *spectrum = em->GetSpectrum();
    
    uint8_t r, g, b;
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            r = MAX(0.0f, (spectrum[3] + 100) * 0.008f - dist(x, y, rlx, rly)) * 0x1F;
            g = MAX(0.0f, (spectrum[4] + 100) * 0.006f - dist(x, y, glx, gly)) * 0x1F;
            b = MAX(0.0f, (spectrum[5] + 100) * 0.008f - dist(x, y, blx, bly)) * 0x1F;
            c->PutPixel(x, y, COLOR_B(MIN(r, 0x1F), MIN(g, 0x1f), MIN(b, 0x1f)));
        }
    }
    
    rstep += spectrum[3] >> 8;
    bstep += spectrum[4] >> 8;
    gstep += spectrum[5] >> 8;
    
    return 1;
}

//int CheckerBoard(Canvas *c, EffectManager *em, char mode)
//{
//    // Sub-pixel
//    static unsigned char offsetX = 0;
//
//    static unsigned char offsetY = 0;
//
//    static Channel_t n = 0;
//    static char currentMode = -1;
//    if (mode != currentMode)
//    {
//        // Performed only once.
//        switch(mode)
//        {
//        case EFFECTMODE_INTRO:
//            n = 0;
//            break;
//        case EFFECTMODE_LOOP:
//            break;
//        case EFFECTMODE_OUTRO:
//            break;
//        }
//        currentMode = mode;
//    } else {
//        // 0b00011111 = 0x1F
//        n = (n+1) & 0x1F;
//    }
//
//    Color_t color = COLOR(n,n,n);
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        char offset = offsetX+offsetY+y;
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            bool draw = ((x+offset) & 0x1) == 0;
//            if (draw)
//            {
//                c->PutPixel(x,y, color);
//            } else {
//                c->PutPixel(x,y, 0);
//            }
//        }
//        
//    }
//    if (n == 0)
//    {
//        offsetX++;
//        //if ((offsetX & 3) == 3)
//        //{
//        //    offsetY++;
//        //}
//    }
//    return 1;
//}


int BlitzyIdle(Canvas *c, EffectManager *em, char mode)
{
    static bool n = false;

    n = !n;
    c->Clear(0);
    c->ClearCeiling(COLOR_CEILING(160,0));
    if (em->GetRandomNumber() > 0xC000)
    {
        c->PutPixelCeiling(1, 0);
    }
    if (em->GetRandomNumber() > 0xD000)
    {
        c->PutPixelCeiling(4, COLOR_CEILING(200,0));
    }
    return 1;
}


int RingRainbow(Canvas *c, EffectManager *em, char mode)
{
    static Color_t colors[8];
    static uint8_t band_pos = 0;
    static uint8_t cw_pos = 0;
    
    // shift color array
    for(char i = 8; --i >= 1;)
        colors[i] = colors[i - 1];
    
    // assign new color
    if(band_pos < 3){
        colors[0] = colorwheel_lut[cw_pos];
        if(band_pos == 2)
            cw_pos = (cw_pos + 6) & 31;
    }
    else{
        colors[0] = 0;
    }
    if(++band_pos > 7)
        band_pos = 0;
    
    // draw to pixels
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            int d = int(distInt(x * 2, y * 2, 7, 9)) >> 1;
            c->PutPixel(x, y, colors[d & 7]);
        }
    }
    
    return 1;
}

int RingRadio(Canvas *c, EffectManager *em, char mode)
{
    static Color_t colors[8];
    static uint8_t band_pos = 0;
    
    // shift color array
    for(char i = 8; --i >= 1;)
        colors[i] = colors[i - 1];
    
    // assign new color
    if(band_pos < 3)
        colors[0] = COLOR_B(28, 28, 28);
    else
        colors[0] = 0;
    if(++band_pos > 7)
        band_pos = 0;
    
    // draw to pixels
    for(char y = 0; y < CANVAS_HEIGHT; y++){
        for(char x = 0; x < CANVAS_WIDTH; x++){
            int d = int(dist(x * 2, y * 2, 7, 9));
            c->PutPixel(x, y, colors[d & 7]);
        }
    }
    
    return 1;
}

int RingFlash(Canvas *c, EffectManager *em, char mode)
{
    static bool bright = false;
    if (bright)
    {
        c->Clear(COLOR(28, 28, 28));
    } else {
        c->Clear(0);
    }
    bright = !bright;
/*
    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
    {
        Color_t color = em->GetRandomNumber();//COLOR(r,g,b);
        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
        {
            c->PutPixel(x,y, color);
        }
    }
*/
    return 1;
}

int RingSolid(Canvas *c, EffectManager *em, char mode)
{
    static unsigned short step = 0;
    
    if((step & 0x1) == 0)
        c->Clear(colorwheel_lut[step >> 4]);
    else
        c->Clear(0);
     
    step = (step + 1) & 511;
    
    return 1;
}


int Overtime(Canvas *c, EffectManager *em, char mode)
{
    c->ClearCeiling(COLOR_CEILING(0,255));
    c->Clear(0);
    return 1;
}


//int Glitz(Canvas *c, EffectManager *em, char mode)
//{
//    static char bgColor = 0;
//    static char yOffset = 0;
//
//    Color_t bgRaw = colorwheel_lut[bgColor & 31];
//
//    Channel_t bgR = RED(bgRaw) << 3;
//    Channel_t bgG = GREEN(bgRaw) << 3;
//    Channel_t bgB = BLUE(bgRaw) << 3;
//    yOffset++;
//    bgColor++;
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        Channel_t sin = sin_lut[((y << 1)+yOffset) & 31];
//        
//        Channel_t r = MAX((char)(bgR - sin), 0);
//        Channel_t g = MAX((char)(bgG - sin), 0);
//        Channel_t b = MAX((char)(bgB - sin), 0);
//        Color_t color = COLOR_B(r, g, b);
//
//        //Color_t rowBG = COLOR_B(r, g, b);
//
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            //Color_t rowBG = colorwheel_lut[(yOffset+y+x) & 31];
//            //Channel_t r = MAX((char)(bgR >> (y >> 2)), 0);
//            //Channel_t g = MAX((char)(bgG >> (y >> 2)), 0);
//            //Channel_t b = MAX((char)(bgB >> (y >> 2)), 0);
//            unsigned short r = em->GetRandomNumber();
//            if (r < 1024)
//            {
//                c->PutPixel(x,y, COLOR_WHITE);
//            }else {
//                c->PutPixel(x,y, color);
//            }
//        }
//        
//    }   
//}

//int Barrel(Canvas *c, EffectManager *em, char mode)
//{
//    static char bgColor = 0;
//    static char yOffset = 0;
//
//    Color_t bgRaw = colorwheel_lut[bgColor & 31];
//
//    Channel_t bgR = RED(bgRaw) << 3;
//    Channel_t bgG = GREEN(bgRaw) << 3;
//    Channel_t bgB = BLUE(bgRaw) << 3;
//    yOffset++;
//    bgColor++;
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        Channel_t sin = sin_lut[((y << 1)+yOffset) & 31];
//        
//        Channel_t r = MAX((char)(bgR - sin), 0);
//        Channel_t g = MAX((char)(bgG - sin), 0);
//        Channel_t b = MAX((char)(bgB - sin), 0);
//        Color_t color = COLOR_B(r, g, b);
//
//        //Color_t rowBG = COLOR_B(r, g, b);
//
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            //Color_t rowBG = colorwheel_lut[(yOffset+y+x) & 31];
//            //Channel_t r = MAX((char)(bgR >> (y >> 2)), 0);
//            //Channel_t g = MAX((char)(bgG >> (y >> 2)), 0);
//            //Channel_t b = MAX((char)(bgB >> (y >> 2)), 0);
//
//            c->PutPixel(x,y, color);
//        }
//        
//    }   
//}


//int SoulWavesVertical(Canvas *c, EffectManager *em, char mode)
//{
//    static char bgColor = 0;
//    static char yOffset = 0;
//    static char xOffset = 0;
//
//    Color_t bgRaw = colorwheel_lut[bgColor & 31];
//
//    Channel_t bgR = RED(bgRaw) << 3;
//    Channel_t bgG = GREEN(bgRaw) << 3;
//    Channel_t bgB = BLUE(bgRaw) << 3;
//    yOffset++;
//    bgColor++;
//
//    if (yOffset & 1)
//    {
//        xOffset++;
//    }
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        Channel_t sin = sin_lut[((y << 1)+yOffset+4) & 31];
//        
//
//        //Color_t rowBG = COLOR_B(r, g, b);
//        
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            Channel_t sin2 = sin_lut[((x << 1)+xOffset+4) & 31];
//            unsigned char theSin = (sin - sin2);
//            Channel_t r = MAX(bgR - theSin, 0) >> 3;
//            Channel_t g = MAX(bgG - theSin, 0) >> 3;
//            Channel_t b = MAX(bgB - theSin, 0) >> 3;
//            Color_t color = COLOR_B(r, g, b);
//
//            c->PutPixel(x,y, color);
//        }
//        
//    }   
//}


//int TheCMYKShuffle(Canvas *c, EffectManager *em, char mode)
//{
//    static char bgColor = 0;
//    static char bgColor2 = 0;
//    static char xOffset = 0;
//    static char yOffset = 0;
//
//    Color_t bgRaw = colorwheel_lut[bgColor & 31];
//
//    Channel_t bgR = RED(bgRaw) << 3;
//    Channel_t bgG = GREEN(bgRaw) << 3;
//    Channel_t bgB = BLUE(bgRaw) << 3;
//
//    Color_t bgRaw2 = colorwheel_lut[bgColor2 & 31];
//
//    Channel_t bgR2 = RED(bgRaw2) << 3;
//    Channel_t bgG2 = GREEN(bgRaw2) << 3;
//    Channel_t bgB2 = BLUE(bgRaw2) << 3;
//
//    xOffset++;
//    bgColor++;
//
//    if (xOffset & 1)
//    {
//        bgColor2++;
//    }
//
//    if ((xOffset & 3) == 0)
//    {
//        yOffset++;
//    }
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        Channel_t sin = sin_lut[((y << 3)+yOffset) & 31];
//
//        //Color_t rowBG = COLOR_B(r, g, b);
//        
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            Channel_t sin2 = sin_lut[((x << 3)+xOffset+4) & 31];
//
//            Channel_t r = MAX(bgR - sin2, 0) >> 3;
//            Channel_t g = MAX(bgG - sin2, 0) >> 3;
//            Channel_t b = MAX(bgB - sin2, 0) >> 3;
//            
//            Channel_t r2 = MAX(bgR2 - sin, 0) >> 3;
//            Channel_t g2 = MAX(bgG2 - sin, 0) >> 3;
//            Channel_t b2 = MAX(bgB2 - sin, 0) >> 3;
//            Color_t color = COLOR( MAX31(r + r2), MAX31(g + g2), MAX31(b + b2));
//            if (color == COLOR_WHITE)
//            { 
//                c->PutPixel(x,y, 0);
//            } else {
//                c->PutPixel(x,y, color);
//            }
//        }
//    }   
//}


//int SoothingRiser(Canvas *c, EffectManager *em, char mode)
//{
//    static char bgColor = 0;
//    static char yOffset = 0;
//
//    Color_t bgRaw = colorwheel_lut[bgColor & 31];
//
//    Channel_t bgR = RED(bgRaw) << 3;
//    Channel_t bgG = GREEN(bgRaw) << 3;
//    Channel_t bgB = BLUE(bgRaw) << 3;
//    yOffset++;
//    bgColor++;
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        Channel_t sin = sin_lut[((y << 1)+yOffset+4) & 31];
//        
//        Channel_t r = MAX(bgR - sin, 0) >> 3;
//        Channel_t g = MAX(bgG - sin, 0) >> 3;
//        Channel_t b = MAX(bgB - sin, 0) >> 3;
//        Color_t color = COLOR_B(r, g, b);
//
//        //Color_t rowBG = COLOR_B(r, g, b);
//
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            //Color_t rowBG = colorwheel_lut[(yOffset+y+x) & 31];
//            //Channel_t r = MAX((char)(bgR >> (y >> 2)), 0);
//            //Channel_t g = MAX((char)(bgG >> (y >> 2)), 0);
//            //Channel_t b = MAX((char)(bgB >> (y >> 2)), 0);
//
//            c->PutPixel(x,y, color);
//        }
//        
//    }   
//}


//int LightTornado(Canvas *c, EffectManager *em, char mode)
//{
//    static int xOffset = 0;
//    static int xOffsetVel = 0;
//    static char xOffsetAcc = 1;
//    static char yOffset = 0;
//
//    static char bgColor = 0;
//
//    //static char count = 0;
//
//    Color_t bgRaw = colorwheel_lut[bgColor];
//
//    Channel_t bgR = RED(bgRaw);
//    Channel_t bgG = GREEN(bgRaw);
//    Channel_t bgB = BLUE(bgRaw);
//    
//    //if (n & 1)
//    {
//        bgColor++;
//        //yOffset++;
//        //yOffset
//        xOffset += (xOffsetVel >> 4);
//        //n = 0;
//    }
//
//    //if (n & 1)
//    {
//        xOffsetVel += xOffsetAcc;
//
//        if ( (xOffsetAcc > 0) && (xOffsetVel > 1024))
//        {
//            xOffsetAcc = -1;
//        } 
//        else if ( (xOffsetAcc < 0) && (xOffsetVel < -1024))
//        {
//            xOffsetAcc = 1;
//        }
//    }
//
//    //n++;
//
//    for (unsigned char y = 0; y < CANVAS_HEIGHT; y++)
//    {
//        //Channel_t r = MAX((char)(bgR >> y), 0);
//        //Channel_t g = MAX((char)(bgG >> y), 0);
//        //Channel_t b = MAX((char)(bgB >> y), 0);
//
//        //Color_t rowBG = COLOR_B(r, g, b);
//
//        for (unsigned char x = 0; x < CANVAS_WIDTH; x++)
//        {
//            Color_t rowBG = colorwheel_lut[(yOffset+y+x) & 31];
//            Channel_t r = MAX((char)(bgR >> (y >> 2)), 0);
//            Channel_t g = MAX((char)(bgG >> (y >> 2)), 0);
//            Channel_t b = MAX((char)(bgB >> (y >> 2)), 0);
//            Color_t color = COLOR_B(r, g, b);
//
//            char n = (x + y + xOffset) & 3;
//            if (n == 0)
//            { 
//                c->PutPixel(x,y, COLOR_WHITE);
//            } else {
//                c->PutPixel(x,y, color);
//            }
//        }
//    }
//}
