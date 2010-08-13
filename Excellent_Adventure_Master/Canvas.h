#ifndef _CANVAS_H
#define _CANVAS_H

// Enable for Simulator
#define USE_UART

//#define BENCHMARK


// Assumes [0, 255]
// Yes, the compiler IS stupid enough to not properly perform shifts without
// corrupting the upper bytes.
//#define COLOR(R,G,B)    (uint32_t)(((uint32_t)(B) << 16) | ((uint32_t)(G) << 8) | (uint32_t)(R))

// Broken:
// #define COLOR(R,G,B)    (uint32_t)(((B) << 16) | ((G) << 8) | (R))

//Produce a byte-packed 15-bit color.  Channels are from [0:31]
#define COLOR(R,G,B)    (Color_t)( ((B) << 10) | ((G) << 5) | ((R)))

#define COLOR_B(R,G,B)  (Color_t)((1 << 15) | ((B) << 10) | ((G) << 5) | ((R)))

// The bright bit is bit-16
#define SETBRIGHT(C)    ((C) |= (1 << 15))
#define UNSETBRIGHT(C)  ((C) &= ~(1 << 15))


// 0b01111100 --> 0x7C

// Color to scaled RGB 256 values

#define RED256(C)       (uint8_t)((((C) << 2) & 0x7C))
#define GREEN256(C)     (uint8_t)((((C) >> 3) & 0x7C))
#define BLUE256(C)      (uint8_t)((((C) >> 8) & 0x7C))

// Bright versions:

// 0b11111000 --> 0xF8

#define RED256_B(C)     (uint8_t)(((C) << 3) & 0xF8)
#define GREEN256_B(C)   (uint8_t)(((C) >> 2) & 0xF8)
#define BLUE256_B(C)    (uint8_t)(((C) >> 7) & 0xF8)

// Raw 5-bit values

#define RED(C)          (uint8_t)(((C))       & 0x1F)
#define GREEN(C)        (uint8_t)(((C) >> 5)  & 0x1F)
#define BLUE(C)         (uint8_t)(((C) >> 10) & 0x1F)

#define IS_BRIGHT(C)    (((C) >> 15) & 0x1)


#define CANVAS_WIDTH        6
#define CANVAS_HEIGHT       10

#define CANVAS_WM1          (CANVAS_WIDTH - 1)
#define CANVAS_HM1          (CANVAS_HEIGHT - 1)


#define CANVAS_WIDTH2       8

#define CANVAS_WIDTH2_BITS  3


#define XY_TO_LINEAR(X,Y)   (((Y) << CANVAS_WIDTH2_BITS) + (X))

#define CANVAS_MEMORY_SIZE  (CANVAS_WIDTH2 * CANVAS_HEIGHT)

#define ADDR_ALL_PIXELS 0

typedef unsigned int Color_t;

class Canvas
{

    // Internally we use a canvas with a height a power of 2
    Color_t *m_canvas;

public:

    Canvas();
    ~Canvas();
    void Destroy();

    // The firstID is the ID of the first pixel in the panels.
    void InitPanels();
    void BlitToPanels();

    void Clear(Color_t color = 0);

    void PutPixel(char x, char y, Color_t color);
    Color_t GetPixel(char x, char y);

    Color_t * GetCanvas();

};

#endif
