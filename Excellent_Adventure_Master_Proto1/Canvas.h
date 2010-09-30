#ifndef _CANVAS_H
#define _CANVAS_H

#define USE_UART 0


// Assumes [0, 255]
// Yes, the compiler IS stupid enough to not properly perform shifts without
// corrupting the upper bytes.
//#define COLOR(R,G,B) (uint32_t)( ((uint32_t)(B) << 16) | ((uint32_t)(G) << 8) | (uint32_t)(R) )
// 
// #define RED(C)   (uint8_t)( (C) & 0xFF )
// #define GREEN(C) (uint8_t)( ((C & 0x00FF00) >> 8) & 0xFF)
// #define BLUE(C)  (uint8_t)( ((C & 0xFF0000) >> 16) & 0xFF)
// 
// #define XY_TO_LINEAR(X,Y) (((Y) << widthShift) + (X))


class Canvas
{
    char width, height;
    int  memorySize;

    // Internally we use a canvas with a height a power of 2
    char widthShift;
    uint8_t *pixels_r, *pixels_g, *pixels_b;
    char firstPixel;

public:
    Canvas();
    ~Canvas();
    Canvas(char width, char height);
    
    void Init(char width, char height);
    void Destroy();

    // The firstID is the ID of the first pixel in the panels.
    void InitPanels(char firstID = 1);
    void BlitToPanels();

    void Clear(uint8_t r=0, uint8_t g=0, uint8_t b=0);

    void PutPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void PutPixel(int i, uint8_t r, uint8_t g, uint8_t b);
    uint8_t GetR(int x, int y);
    uint8_t GetG(int x, int y);
    uint8_t GetB(int x, int y);

    uint32_t* GetCanvas();
    char GetHeight();
    char GetWidth();
};

#endif

