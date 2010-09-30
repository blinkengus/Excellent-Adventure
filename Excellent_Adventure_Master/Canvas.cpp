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

#include <WProgram.h>
#include "Canvas.h"
#include "Globals.h"
// If we wish to use dynamically allocated memory.

#ifdef USE_ARDUINO
#include <Wire.h>

//#define BLINKM_PLUGGED_INTO_ARDUINO
//#define SERIAL  (Serial)

#define I2C_INIT(RATE)                                                      \
    Wire.begin()

#define I2C_WRITE(ADDR, DATA, LENGTH)                                       \
    Wire.beginTransmission((unsigned char)(ADDR));                          \
    Wire.send((DATA),(LENGTH));                                             \
    Wire.endTransmission();

#define I2C_WRITE1(DATA)                                                    \
    Wire.send(DATA)

#define I2C_BEGIN_TRANSMISSION(ADDR)                                        \
    Wire.beginTransmission((unsigned char)(ADDR))

#define I2C_END_TRANSMISSION                                                \
    Wire.endTransmission()
    

#endif

#ifdef USE_MAPLE
#include <wirish.h>
#include <i2c.h>

#define I2C_INIT(RATE)                                                      \
    i2c_init(I2C_PORT1, (RATE))

#define I2C_WRITE(ADDR, DATA, LENGTH)                                       \
    i2c_master_write(I2C_PORT1,(ADDR),(DATA),(LENGTH))

#define I2C_WRITE1(DATA)                                                    \
    I2C_WRITE(I2C_PORT1,&(DATA),1)

#define I2C_BEGIN_TRANSMISSION(ADDR)
#define I2C_END_TRANSMISSION


#endif

// Canvas-to-I2C address translation table.
// A negative 1 indicates that the entry should be skipped.

static char panelAddresses[] = 
{
    10,    12,  13,    15,  16,    18,
    19,    21,  22,    24,  25,    27,
    28,    30,  31,    33,  34,    36,
    37,    39,  40,    -1,  -1,    45,
    46,    48,  49,    -1,  -1,    54,
    55,    -1,  -1,    -1,  -1,    63,
    64,    -1,  -1,    -1,  -1,    72,
    73,    -1,  -1,    78,  79,    81,
    82,    84,  85,    87,  88,    90,
    91,    93,  94,    96,  97,    99
};

//     0       // Goes around the edge first
//  3  4  1
//     2

static char ceilingAddresses[] = 
{
    1,  4,  5,  2,  3
};


// Minibooth:
/*
static char panelAddresses[] =
{
    1, 2, 3, 4, 5, 6,
    7, 8, 9, 10, 11, 12,
    13, 14, 15, 16, 17, 18,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1
};
*/



/*
extern "C" 
{ 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
*/

Canvas :: Canvas()
{
    m_who = 0;
#ifdef MEMORY_DYNAMIC    
    m_ceiling = (Color_t *)malloc(sizeof(Color_t) * CEILING_LIGHTS);

#ifdef CANVAS_PAGES
    m_canvas[0] = (Color_t *)malloc(sizeof(Color_t) * CANVAS_MEMORY_SIZE);
    m_canvas[1] = (Color_t *)malloc(sizeof(Color_t) * CANVAS_MEMORY_SIZE);
#else
    m_canvas = (Color_t *)malloc(sizeof(Color_t) * CANVAS_MEMORY_SIZE);
#endif

#endif
    Clear();
}

Canvas :: ~Canvas()
{
    Destroy();
}

void Canvas :: Destroy()
{
#ifdef MEMORY_DYNAMIC    
    free(m_ceiling);

#ifdef CANVAS_PAGES
    free(m_canvas[0]);
    free(m_canvas[1]);
#else
    free(m_canvas);
#endif

#endif
}

void Canvas :: InitPanels ()
{

#ifdef BLINKM_PLUGGED_INTO_ARDUINO
    // Use this if the BlinkM is connected to the Arduino for power.
    // Otherwise regular I2C commands will work.

    // BlinkM_beginWithPower();
    // Copy of what is called by the BlinkM_funcs.h version:
    DDRC |= _BV(PORTC3) | _BV(PORTC2);  // make outputs
    PORTC &=~ _BV(PORTC2);
    PORTC |=  _BV(PORTC3);
    delay(100);
#endif

    I2C_INIT(I2C_RATE);
    FadeToBlack();
    /*
    unsigned char data[4];
    // Stop all scripts
    data[0] = 'o';
    I2C_WRITE(ADDR_ALL_PIXELS, data, 1);

    // Set fade speed
    data[0] = 'f';
    data[1] = 192;
    I2C_WRITE(ADDR_ALL_PIXELS, data, 2);

    // Fade to black
    data[0] = 'c';
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    I2C_WRITE(ADDR_ALL_PIXELS, data, 4);
    */
}

void Canvas :: ReinitPanels()
{
    I2C_BEGIN_TRANSMISSION(ADDR_ALL_PIXELS);
    I2C_WRITE1('o');
    I2C_END_TRANSMISSION;

    // Set fade speed
    //data[0] = 'f';
    //data[1] = 16;
    //I2C_WRITE(ADDR_ALL_PIXELS, data, 2);

    I2C_BEGIN_TRANSMISSION(ADDR_ALL_PIXELS);
    I2C_WRITE1('f');
    I2C_WRITE1(192);
    I2C_END_TRANSMISSION;
}


void Canvas :: BlitToPanelsInterpolate
(
    bool                                            page,
    char                                            weight
)
{
    Channel_t RGB[4];
    char who0 = page ? 0 : 1;
    char who1 = page ? 1 : 0;

#ifdef BENCHMARK    
    RGB[3] = 253;
    SERIAL_WRITE(RGB, 4);
#endif

#ifdef USE_UART
    // This is a sync frame:

    RGB[0] = RGB[1] = RGB[2] = RGB[3] = 0xFF;
    SERIAL_WRITE(RGB, 4);
#else
    // Sets the color immediately ('n')
    // Fade to color ('c')

    RGB[0] = 'o';
    I2C_WRITE(ADDR_ALL_PIXELS, RGB, 1);
    RGB[0] = 'c';
#endif
    // Obtain the first address of our panels:
    char *addr = &panelAddresses[0];

    for (char y = 0; y < CANVAS_HEIGHT; y++)
    {
        //memory = (canvas + (y << widthShift));
        char x = 0;
        while (x < CANVAS_WIDTH)
        {
            // We skip any addresses that are < 0, saving processing.
            if (*addr >= 0)
            {
                SetCanvasPage(who0);
                Color_t color = GetPixel(x, y);

                Channel_t r0, r1, g0, g1, b0, b1;
                
                if (IS_BRIGHT(color))
                {
                    // Set color immediately:
                    r0 = RED256_B(color);
                    g0 = GREEN256_B(color);
                    b0 = BLUE256_B(color);
                    
                } else {
                    r0 = RED256(color);
                    g0 = GREEN256(color);
                    b0 = BLUE256(color);
                }

                SetCanvasPage(who1);
                color = GetPixel(x, y);

                if (IS_BRIGHT(color))
                {
                    // Set color immediately:
                    r1 = RED256_B(color);
                    g1 = GREEN256_B(color);
                    b1 = BLUE256_B(color);
                    
                } else {
                    r1 = RED256(color);
                    g1 = GREEN256(color);
                    b1 = BLUE256(color);
                }

                RGB[1] = (Channel_t)(CHANNEL_LINTERP(r1,r0,weight) && 0xFF);
                RGB[2] = (Channel_t)(CHANNEL_LINTERP(g1,g0,weight) && 0xFF);
                RGB[3] = (Channel_t)(CHANNEL_LINTERP(b1,b0,weight) && 0xFF);
                

#ifdef USE_UART
                RGB[0] = (unsigned)(*addr); //((x == 0) && (y == 0)) ? 1 : 0;
                SERIAL_WRITE(RGB, 4);
#else
                
                I2C_WRITE((unsigned)(*addr), &RGB[0], 4);
#endif

            }
            addr++;
            x++;
        }
    }
#ifdef BENCHMARK    
    RGB[0] = 254;
    SERIAL_WRITE(RGB, 4);
#endif

}



void Canvas :: BlitToPanels()
{
    Channel_t RGB[4];
#ifdef BENCHMARK    
    RGB[3] = 253;
    SERIAL_WRITE(RGB, 4);
#endif

#ifdef USE_PANEL_SIMULATOR
//#ifdef USE_UART
    // This is a sync frame:

    RGB[0] = RGB[1] = RGB[2] = RGB[3] = 0xFF;
    SERIAL_WRITE(RGB, 4);
//#endif
#else
    // Sets the color immediately ('n')
    // Fade to color ('c')

    //RGB[0] = 'o';
    //I2C_WRITE(ADDR_ALL_PIXELS, RGB, 1);

    ReinitPanels();
    RGB[0] = 'c';
#endif
    // Obtain the first address of our panels:
    for (char y = 0, *addr = &panelAddresses[0]; y < CANVAS_HEIGHT; y++)
    {
        //memory = (canvas + (y << widthShift));
        char x = 0;
        while (x < CANVAS_WIDTH)
        {
            // We skip any addresses that are < 0, saving processing.
            if (*addr >= 0)
            {

                Color_t color = GetPixel(x, y);
                //if (IS_BRIGHT(color))
                //{
                    // Set color immediately:
                    RGB[1] = RED256_B(color);
                    RGB[2] = GREEN256_B(color);
                    RGB[3] = BLUE256_B(color);
                    
                //} else {
                //    RGB[1] = RED256(color);
                //    RGB[2] = GREEN256(color);
                //    RGB[3] = BLUE256(color);
                //}
#ifdef USE_PANEL_SIMULATOR
//#ifdef USE_UART
                RGB[0] = (unsigned)(*addr); //((x == 0) && (y == 0)) ? 1 : 0;
                SERIAL_WRITE(RGB, 4);
//#endif
#else               
                //I2C_WRITE((unsigned)(*addr), RGB, 4);
                I2C_BEGIN_TRANSMISSION(*addr);
                I2C_WRITE1(RGB[0]);
                I2C_WRITE1(RGB[1]);
                I2C_WRITE1(RGB[2]);
                I2C_WRITE1(RGB[3]);
                I2C_END_TRANSMISSION;
#endif

            }
            addr++;
            x++;
        }
    }
#ifndef CEILING_LIGHTS_DISABLE
    // Send out our ceiling data too:
    for (char n = 0, *addr = &ceilingAddresses[0]; n < CEILING_LIGHTS; n++, addr++)
    {
        ColorCeiling_t color = m_ceiling[n];
        RGB[1] = CEILING_WHITE(color);
        RGB[2] = CEILING_UV(color);
        RGB[3] = 0;
#ifdef USE_PANEL_SIMULATOR
//#ifdef USE_UART
        RGB[0] = (unsigned)(*addr); //((x == 0) && (y == 0)) ? 1 : 0;
        SERIAL_WRITE(RGB, 4);
//#endif
#else
        //I2C_WRITE((unsigned)(*addr), RGB, 4);
        I2C_BEGIN_TRANSMISSION(*addr);
        I2C_WRITE1(RGB[0]);
        I2C_WRITE1(RGB[1]);
        I2C_WRITE1(RGB[2]);
        I2C_WRITE1(RGB[3]);
        I2C_END_TRANSMISSION;
#endif
    }
#endif

#ifdef BENCHMARK    
    RGB[0] = 254;
    SERIAL_WRITE(RGB, 4);
#endif
}

void Canvas :: Clear
(
    Color_t                                         color
)
{
//#ifdef USE_ARDUINO    
//    memset(&m_canvas[m_who][0], color, sizeof(Color_t)*CANVAS_MEMORY_SIZE);
//#else
    // For some reason the Maple does not support this *fundamental* function,
    // or I can't find it.

#ifdef CANVAS_PAGES
    Color_t *memory =       &m_canvas[m_who][0];
    Color_t *end =          &m_canvas[m_who][CANVAS_MEMORY_SIZE];
#else
    Color_t *memory =       &m_canvas[0];
    Color_t *end =          &m_canvas[CANVAS_MEMORY_SIZE];
#endif
    do
    {
        *(memory++) = color;
    } while (memory != end);

//#endif
}

void Canvas :: ClearCeiling
(
    ColorCeiling_t                                  color
)
{
//#ifdef USE_ARDUINO    
//    memset(&m_ceiling[0], color, sizeof(ColorCeiling_t)*CEILING_LIGHTS);
//#else
    // For some reason the Maple does not support this *fundamental* function,
    // or I can't find it.
    ColorCeiling_t *memory =       &m_ceiling[0];
    ColorCeiling_t *end =          &m_ceiling[CEILING_LIGHTS];
    do
    {
        *(memory++) = color;
    } while (memory != end);

//#endif
}



void Canvas :: FadeToBlack()
{




    //unsigned char data[4];

    // Stop all scripts
    //data[0] = 'o';
    //I2C_WRITE(ADDR_ALL_PIXELS, data, 1);

    I2C_BEGIN_TRANSMISSION(ADDR_ALL_PIXELS);
    I2C_WRITE1('o');
    I2C_END_TRANSMISSION;

    // Set fade speed
    //data[0] = 'f';
    //data[1] = 16;
    //I2C_WRITE(ADDR_ALL_PIXELS, data, 2);

    I2C_BEGIN_TRANSMISSION(ADDR_ALL_PIXELS);
    I2C_WRITE1('f');
    I2C_WRITE1(50);
    I2C_END_TRANSMISSION;


    // Fade to black
    //data[0] = 'c';
    //data[1] = 0;
    //data[2] = 0;
    //data[3] = 0;
    //I2C_WRITE(ADDR_ALL_PIXELS, data, 4);

    I2C_BEGIN_TRANSMISSION(ADDR_ALL_PIXELS);
    I2C_WRITE1('c');
    I2C_WRITE1(0);
    I2C_WRITE1(0);
    I2C_WRITE1(0);
    I2C_END_TRANSMISSION;

}


void Canvas :: SetCanvasPage
(
    char                                            who
)
{
    m_who = (who == 1) ? 1 : 0;
}

void Canvas :: PutPixelCeiling
(
    char                                            n,
    ColorCeiling_t                                  color
)
{
    // Ex:
    // width = 14, height = 12
    // wS = 4
    //
    // x = 3, y = 7
    // 7 << wS = 112 // 8th row of 16px wide
    // 112 + 3 = 115 // 4th col of that row

    m_ceiling[n] = color;
}


void Canvas :: PutPixel
(
    char                                            x,
    char                                            y,
    Color_t                                         color
)
{
    // Ex:
    // width = 14, height = 12
    // wS = 4
    //
    // x = 3, y = 7
    // 7 << wS = 112 // 8th row of 16px wide
    // 112 + 3 = 115 // 4th col of that row
#ifdef CANVAS_PAGES
    m_canvas[m_who][XY_TO_LINEAR(x,y)] = color;
#else
    m_canvas[XY_TO_LINEAR(x,y)] = color;
#endif
}

Color_t Canvas :: GetPixel
(
    char                                            x,
    char                                            y
)
{
    // Ex:
    // width = 14, height = 12
    // wS = 4
    //
    // x = 3, y = 7
    // 7 << wS = 112 // 8th row of 16px wide
    // 112 + 3 = 115 // 4th col of that row

#ifdef CANVAS_PAGES
    return m_canvas[m_who][XY_TO_LINEAR(x,y)];
#else
    return m_canvas[XY_TO_LINEAR(x,y)];
#endif
}

ColorCeiling_t Canvas :: GetPixelCeiling
(
    char                                            n
)
{

    return m_ceiling[n];
}

Color_t * Canvas :: GetCanvas()
{
#ifdef CANVAS_PAGES
    return &m_canvas[m_who][0];
#else
    return &m_canvas[0];
#endif
}


