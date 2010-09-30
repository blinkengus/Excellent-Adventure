/*
    ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
   /\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
   \ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
    \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
     \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   

    www.carbonworkshop.com/bm10                                 June 11, 2010

    DESCRIPTION
    
        This is a memory "canvas" that functions as "video memory" for the 
        I2C LED pixel array (BlinkM/ThingM protocol).

        Just like a double-buffer for computer graphics, the canvas is drawn 
        to using elementary operations, and then the contents are "blitted" 
        (copied) to the video pixels in a single operation.

    AUTHORS

        Christopher Condrat     - chris AT g6net DOT com
        Gustavo Huber           - gush AT carbonworkshop DOT com

        (this list is merely for whom to contact if you need help with this 
        specific code, not all contributing members)

    CONTRIBUTORS

        Ryan Alexander          - scloopy AT gmail DOT com
        Christopher Condrat     - chris AT g6net DOT com
        Gustavo Huber           - gush AT carbonworkshop DOT com
        Daniel Massey           - pichiste AT gmail DOT com

        (and more!)

    LICENSE

        This work is licensed under the Creative Commons License 
        (CC-BY variant). For a full copy of the license, please consult:

        http://creativecommons.org/licenses/by/3.0/
*/


#ifndef _CANVAS_H
#define _CANVAS_H
#include "Globals.h"

// True canvas width/height

#define CANVAS_WIDTH        6
#define CANVAS_HEIGHT       10

#define CEILING_LIGHTS      5

#define CANVAS_WM1          (CANVAS_WIDTH - 1)
#define CANVAS_HM1          (CANVAS_HEIGHT - 1)

// 2^n -aligned canvas width for faster addressing
#define CANVAS_WIDTH2       8
#define CANVAS_WIDTH2_BITS  3

#define XY_TO_LINEAR(X,Y)   (((Y) << CANVAS_WIDTH2_BITS) + (X))

#define CANVAS_MEMORY_SIZE  (CANVAS_WIDTH2 * CANVAS_HEIGHT)

// I2C address for sending a command to all pixels:
#define ADDR_ALL_PIXELS     0

// We use 16-bit color for now
typedef unsigned short      ColorCeiling_t;
typedef unsigned short      Color_t;
typedef unsigned char       Channel_t;

#define CHANNEL_BITS        5
#define R_SHIFT             0
#define G_SHIFT             CHANNEL_BITS
#define B_SHIFT             (2*CHANNEL_BITS)
#define BRIGHT_SHIFT        (3*CHANNEL_BITS)

#define WHITE_SHIFT         0
#define UV_SHIFT            8

// 0b11111000 --> 0xF8
#define BRIGHT_MASK         0xF8

// 0b01111100 --> 0x7C
#define NORMAL_MASK         0x7C
#define CHANNEL_MASK        0x1F
#define CEILING_CHANNEL_MASK 0x00FF

#define NORMAL_SCALE        (-2)
#define BRIGHT_SCALE        (-3)

// Enable to allow two pages of memory
//#define CANVAS_PAGES        

// Yes, the compiler IS stupid enough to not properly perform shifts without
// corrupting the upper bytes.
//#define COLOR(R,G,B)    (uint32_t)(((uint32_t)(B) << 16) | ((uint32_t)(G) << 8) | (uint32_t)(R))

// Broken:
// #define COLOR(R,G,B)    (uint32_t)(((B) << 16) | ((G) << 8) | (R))

//Produce a byte-packed 15-bit color.  Channels are from [0,31]
#define COLOR(R,G,B)        (Color_t)(      ((B) << B_SHIFT)                \
                                        |   ((G) << G_SHIFT)                \
                                        |   ((R) << R_SHIFT))


#define COLOR_B(R,G,B)      (Color_t)((1 << BRIGHT_SHIFT) | COLOR(R,G,B))

// Colors in the ceiling are [0, 255] not [0,31]

#define COLOR_CEILING(W,U)  \
    (ColorCeiling_t)(       (((Channel_t)(W) & CEILING_CHANNEL_MASK) << WHITE_SHIFT) \
                        |   (((Channel_t)(U) & CEILING_CHANNEL_MASK) << UV_SHIFT))


// The bright bit is bit-16
#define SETBRIGHT(C)        ((C) |= (1 << BRIGHT_SHIFT))
#define UNSETBRIGHT(C)      ((C) &= ~(1 << BRIGHT_SHIFT))

#define sbi(WHO,C)          ((WHO) |= (1 << (C)))
#define cbi(WHO,C)          ((WHO) &= ~(1 << (C)))

// Color to scaled RGB 256 values

//#define RED256(C)         (Channel_t)(((C) >> (signed)(R_SHIFT+NORMAL_SCALE)) & NORMAL_MASK)
//#define GREEN256(C)       (Channel_t)(((C) >> (G_SHIFT+NORMAL_SCALE)) & NORMAL_MASK)
//#define BLUE256(C)        (Channel_t)(((C) >> (B_SHIFT+NORMAL_SCALE)) & NORMAL_MASK)

// Bright versions:

//#define RED256_B(C)         (Channel_t)(((C) >> (signed)((signed)R_SHIFT+(signed)BRIGHT_SCALE)) & BRIGHT_MASK)
//#define GREEN256_B(C)       (Channel_t)(((C) >> (G_SHIFT+BRIGHT_SCALE)) & BRIGHT_MASK)
//#define BLUE256_B(C)        (Channel_t)(((C) >> (B_SHIFT+BRIGHT_SCALE)) & BRIGHT_MASK)

// Idiot compilers don't properly shift negative values

#define RED256(C)       (Channel_t)((((C) << 2)       & NORMAL_MASK))
#define GREEN256(C)     (Channel_t)((((C) >> 3)       & NORMAL_MASK))
#define BLUE256(C)      (Channel_t)((((C) >> 8)       & NORMAL_MASK))

#define RED256_B(C)     (Channel_t)(((C) << 3)        & BRIGHT_MASK)
#define GREEN256_B(C)   (Channel_t)(((C) >> 2)        & BRIGHT_MASK)
#define BLUE256_B(C)    (Channel_t)(((C) >> 7)        & BRIGHT_MASK)

#define CEILING_WHITE(C) \
    (Channel_t)(((ColorCeiling_t)(C) >> WHITE_SHIFT)   & CEILING_CHANNEL_MASK)
#define CEILING_UV(C)    \
    (Channel_t)(((ColorCeiling_t)(C) >> UV_SHIFT)      & CEILING_CHANNEL_MASK)

// Raw 5-bit values

#define RED(C)          (Channel_t)(((C) >> R_SHIFT)  & CHANNEL_MASK)
#define GREEN(C)        (Channel_t)(((C) >> G_SHIFT)  & CHANNEL_MASK)
#define BLUE(C)         (Channel_t)(((C) >> B_SHIFT)  & CHANNEL_MASK)
                                                      
#define IS_BRIGHT(C)    (((C) >> BRIGHT_SHIFT)        & 0x1)

// Color0 Color1, Weight, DENOM (as shift)
#define CHANNEL_LINTERP_DENOM   5
#define CHANNEL_LINTERP(C0,C1,W) ((int)((int)((int)(C1-C0)*W) >> CHANNEL_LINTERP_DENOM) + C0)

#define COLOR_WHITE     0x7FFF

class Canvas
{

    // Internally we use a canvas with a height a power of 2

#ifdef MEMORY_DYNAMIC
 #ifdef CANVAS_PAGES
    Color_t                                     *   m_canvas[2];
 #else
    Color_t                                     *   m_canvas;
 #endif
    Color_t                                     *   m_ceiling;
#else

 #ifdef CANVAS_PAGES
    Color_t                                         m_canvas[2][CANVAS_MEMORY_SIZE];
 #else
    Color_t                                         m_canvas[CANVAS_MEMORY_SIZE];
 #endif


    Color_t                                         m_ceiling[CEILING_LIGHTS];
#endif
    char                                            m_who;
public:

    Canvas();
    ~Canvas();
    void Destroy();

    // The firstID is the ID of the first pixel in the panels.
    void InitPanels();
    void ReinitPanels();
    void BlitToPanels();
    void BlitToPanelsInterpolate
    (
        bool                                        page,
        char                                        weight
    );
    void Clear
    (
        Color_t                                     color
            = 0
    );
    void ClearCeiling
    (
        ColorCeiling_t                              color
            = 0
    );

    void FadeToBlack();

    // The canvas has two memory pages.
    void SetCanvasPage
    (
        char                                        who
    );

    void PutPixel
    (
        char                                        x,
        char                                        y,
        Color_t                                     color
    );

    void PutPixelCeiling
    (
        char                                        n,
        ColorCeiling_t                              color
    );


    Color_t GetPixel
    (
        char                                        x,
        char                                        y
    );

    ColorCeiling_t GetPixelCeiling
    (
        char                                        n
    );
     

    Color_t * GetCanvas ();

};



#endif
