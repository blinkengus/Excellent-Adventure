#include <Wire.h>
#include <WProgram.h>
#include "Canvas.h"

/*
extern "C"
 {
 #include "utility/twi.h"    // from Wire library, so we can do bus scanning
 }
 */

Canvas::Canvas()
{
}

Canvas::Canvas(char width, char height)
{
    Init(width, height);
}

void Canvas::Init(char width, char height)
{
    this->width    = width;
    this->height = height;
    widthShift = (width > 64) ? 7
               : (width > 32) ? 6
               : (width > 16) ? 5
               : (width >  8) ? 4
               : (width >  4) ? 3
               : (width >  2) ? 2
               : (width >  1) ? 1
               : 0;
    memorySize = (1 << widthShift) * height;
    canvas = (uint32_t *)malloc(sizeof(uint32_t) * memorySize);
    canvasEnd = canvas + memorySize;
    Clear();
}

Canvas::~Canvas()
{
    Destroy();
}

void Canvas::Destroy()
{
    free(canvas);
}

void Canvas::InitPanels(char firstID)
{
    firstPixel = firstID;
    Wire.begin();
    Wire.beginTransmission(0); //replaces BlinkM_stopScript(0)
    Wire.send('o');
    Wire.endTransmission();
}

void Canvas::BlitToPanels()
{
#ifdef USE_UART
    static uint8_t RGB[4];
    uint8_t pixel = 0;

    RGB[0] = RGB[1] = RGB[2] = RGB[3] = 255;
    Serial.write(RGB, 4);

#else
    uint8_t pixel = firstPixel;
#endif

    for(char y = 0; y < height; y++){
        //memory = (canvas + (y << widthShift));
        for (char x = 0; x < width; x++){
            uint32_t color = GetPixel(x,y);
#ifdef USE_UART
            RGB[0] = RED(color);
            RGB[1] = GREEN(color);
            RGB[2] = BLUE(color);
            RGB[3] = pixel; //((x == 0) && (y == 0)) ? 1 : 0;
            Serial.write(RGB, 4);
#else
            Wire.beginTransmission(pixel);
            Wire.send('n');
            Wire.send(RED(color));
            Wire.send(GREEN(color));
            Wire.send(BLUE(color));
            Wire.endTransmission();
#endif
            pixel++;
        }
    }
}

void Canvas::Clear(uint32_t color)
{
    memset(canvas, color, sizeof(uint32_t)*memorySize);

    //long *memory = canvas;
    //do
    //{
    //        *memory = color;
    //        memory++;
    //} while (memory != canvasEnd);
}

void Canvas::PutPixel(int x, int y, uint32_t color)
{
    // Ex:
    // width = 14, height = 12
    // wS = 4
    //
    // x = 3, y = 7
    // 7 << wS = 112 // 8th row of 16px wide
    // 112 + 3 = 115 // 4th col of that row

    canvas[XY_TO_LINEAR(x, y)] = color;
}

uint32_t Canvas::GetPixel(int x, int y)
{
    // Ex:
    // width = 14, height = 12
    // wS = 4
    //
    // x = 3, y = 7
    // 7 << wS = 112 // 8th row of 16px wide
    // 112 + 3 = 115 // 4th col of that row

    return canvas[XY_TO_LINEAR(x, y)];
}


inline uint32_t* Canvas::GetCanvas()
{
    return canvas;
}


char Canvas::GetWidth()
{
    return width;
}
char Canvas::GetHeight()
{
    return height;
}
