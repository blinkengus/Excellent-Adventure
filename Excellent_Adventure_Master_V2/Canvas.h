#ifndef _CANVAS_H
#define _CANVAS_H

#define USE_UART 1


// Assumes [0, 255]
// Yes, the compiler IS stupid enough to not properly perform shifts without
// corrupting the upper bytes.
#define COLOR(R,G,B) (uint32_t)( ((uint32_t)(B) << 16) | ((uint32_t)(G) << 8) | (uint32_t)(R) )

// Broken:
// #define COLOR(R,G,B)    (uint32_t)(((B) << 16) | ((G) << 8) | (R))


#define RED(C)   (uint8_t)( (C) & 0xFF )
#define GREEN(C) (uint8_t)( ((C & 0x00FF00) >> 8) & 0xFF)
#define BLUE(C)  (uint8_t)( ((C & 0xFF0000) >> 16) & 0xFF)

#define XY_TO_LINEAR(X,Y) (((Y) << widthShift) + (X))

class Canvas
{
    char width;
    char height;
    int  memorySize;

    // Internally we use a canvas with a height a power of 2
    char     widthShift;
    uint32_t *canvas;
    uint32_t *canvasEnd;
    char     firstPixel;

public:
    Canvas();
    ~Canvas();
    Canvas(char width, char height);
    
    void Init(char width, char height);
    void Destroy();

    // The firstID is the ID of the first pixel in the panels.
    void InitPanels(char firstID = 1);
    void BlitToPanels();

    void Clear(uint32_t color = 0);

    void PutPixel(int x, int y, uint32_t color);

    uint32_t GetPixel(int x, int y);

    uint32_t* GetCanvas();
    char GetHeight();
    char GetWidth();
};

#endif

