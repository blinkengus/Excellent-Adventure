#include <Wire.h>
#include <WProgram.h>
#include "Canvas.h"
#include "BlinkM_funcs.h"


Canvas::Canvas()
{
}

Canvas::Canvas(char _width, char _height)
{
    Init(_width, _height);
}

void Canvas::Init(char _width, char _height)
{
    width = _width;
    height = _height;
    
    memorySize = width * height;
    
    pixels_r = (uint8_t *)malloc(sizeof(uint8_t) * memorySize);
    pixels_g = (uint8_t *)malloc(sizeof(uint8_t) * memorySize);
    pixels_b = (uint8_t *)malloc(sizeof(uint8_t) * memorySize);
    
    Clear();
}

Canvas::~Canvas()
{
    Destroy();
}

void Canvas::Destroy()
{
    free(pixels_r);
    free(pixels_g);
    free(pixels_b);
}


void Canvas::InitPanels(char firstID)
{
    firstPixel = firstID;
    BlinkM_beginWithPower();
    BlinkM_setRGB(0, 0x00, 0x00, 0x00);
    BlinkM_setFadeSpeed(0, 50);
}

void Canvas::BlitToPanels()
{
// #ifdef USE_UART
//     static uint8_t RGB[4];
//     uint8_t pixel = 0;
// 
//     RGB[0] = RGB[1] = RGB[2] = RGB[3] = 255;
//     Serial.write(RGB, 4);
// 
// #else
    char pixel = firstPixel;
// #endif

    int index;
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            index = y * width + x;
            BlinkM_fadeToRGB( pixel, pixels_r[index], pixels_g[index], pixels_b[index] );
            pixel++;
        }
    }
}

void Canvas::Clear(uint8_t r, uint8_t g, uint8_t b)
{
    memset(pixels_r, r, sizeof(uint8_t) * memorySize);
    memset(pixels_g, g, sizeof(uint8_t) * memorySize);
    memset(pixels_b, b, sizeof(uint8_t) * memorySize);
}


void Canvas::PutPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    int index = y * width + x;
    pixels_r[index] = r;
    pixels_g[index] = g;
    pixels_b[index] = b;
}
void Canvas::PutPixel(int i, uint8_t r, uint8_t g, uint8_t b)
{
    pixels_r[i] = r;
    pixels_g[i] = g;
    pixels_b[i] = b;
}

// void Canvas::PutPixelHSV(int x, int y, char r, char g, char b)
// {
//     
// }

uint8_t Canvas::GetR(int x, int y)
{
    return pixels_r[ y * width + x ];
}
uint8_t Canvas::GetG(int x, int y)
{
    return pixels_g[ y * width + x ];
}
uint8_t Canvas::GetB(int x, int y)
{
    return pixels_b[ y * width + x ];
}


char Canvas::GetWidth()
{
    return width;
}
char Canvas::GetHeight()
{
    return height;
}