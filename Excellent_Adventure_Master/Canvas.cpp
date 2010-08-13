#include <Wire.h>
#include <WProgram.h>
#include "Canvas.h"


#define BLINKM_PLUGGED_INTO_ARDUINO

// A negative 1 indicates that the entry should be skipped.

static char panelAddresses[] =
{
    10,     12,  13,    15,  16,    18,
    19,     21,  22,    24,  25,    27,
    28,     30,  31,    33,  34,    36,
    37,     39,  40,    -1,  -1,    45,
    46,     48,  49,    -1,  -1,    54,
    55,     -1,  -1,    -1,  -1,    63,
    64,     -1,  -1,    -1,  -1,    72,
    73,     -1,  -1,    78,  79,    81,
    82,     84,  85,    87,  88,    90,
    91,     93,  94,    96,  97,    99
};



/*
extern "C"
{
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
*/

Canvas :: Canvas()
{
    m_canvas = (Color_t *)malloc(sizeof(Color_t) * CANVAS_MEMORY_SIZE);
    Clear();
}

Canvas :: ~Canvas()
{
    Destroy();
}

void Canvas :: Destroy()
{
    free(m_canvas);
}

void Canvas::InitPanels()
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

    Wire.begin();

    // Disable all scripts.
    // BlinkM_stopScript(0)
    Wire.beginTransmission(ADDR_ALL_PIXELS);
    Wire.send('o');
    Wire.endTransmission();

    // Set a fade speed
    // BlinkM_setFadeSpeed(0, 50);
    Wire.beginTransmission(ADDR_ALL_PIXELS);
    Wire.send('f');
    Wire.send(50); // Fade speed
    Wire.endTransmission();

    // Set color to fade to (black):
    Wire.beginTransmission(ADDR_ALL_PIXELS);
    Wire.send('c');
    Wire.send(0);
    Wire.send(0);
    Wire.send(0);
    Wire.endTransmission();
}

void Canvas::BlitToPanels()
{

#ifdef USE_UART
    static uint8_t RGB[4];
#else
#ifdef BENCHMARK
    static uint8_t RGB[4];
#endif
#endif
#ifdef USE_UART
    // This is a sync frame:
    RGB[0] = RGB[1] = RGB[2] = RGB[3] = 255;
    Serial.write(RGB, 4);
#endif

#ifdef BENCHMARK
    RGB[3] = 253;
    Serial.write(RGB, 4);
#endif

    // Obtain the first address of our panels:
    char *addr = &panelAddresses[0];

    for(char y = 0; y < CANVAS_HEIGHT; y++){
        //memory = (canvas + (y << widthShift));
        char x = 0;
        while (x < CANVAS_WIDTH){
            // We skip any addresses that are < 0, saving processing.
            if(*addr >= 0){
                Color_t color = GetPixel(x, y);
                if(IS_BRIGHT(color)){
#ifdef USE_UART
                    RGB[0] = RED256_B(color);
                    RGB[1] = GREEN256_B(color);
                    RGB[2] = BLUE256_B(color);
                    RGB[3] = (uint8_t)(*addr); //((x == 0) && (y == 0)) ? 1 : 0;
                    Serial.write(RGB, 4);
#else
                    // Set color immediately:
                    Wire.beginTransmission((uint8_t)(*addr));
                    Wire.send('n');
                    Wire.send(RED256_B(color));
                    Wire.send(GREEN256_B(color));
                    Wire.send(BLUE256_B(color));
                    Wire.endTransmission();
                    //delay(100);
#endif
                }
                else{
#ifdef USE_UART
                    RGB[0] = RED256(color);
                    RGB[1] = GREEN256(color);
                    RGB[2] = BLUE256(color);
                    RGB[3] = (uint8_t)(*addr); //((x == 0) && (y == 0)) ? 1 : 0;
                    Serial.write(RGB, 4);
#else
                    // Set color immediately:
                    Wire.beginTransmission((uint8_t)(*addr));
                    Wire.send('n');
                    Wire.send(RED256(color));
                    Wire.send(GREEN256(color));
                    Wire.send(BLUE256(color));
                    Wire.endTransmission();
#endif
                }
            }
            addr++;
            x++;
        }
    }
#ifdef BENCHMARK
    RGB[3] = 254;
    Serial.write(RGB, 4);
#endif
}

void Canvas::Clear(Color_t color)
{
    memset(m_canvas, color, sizeof(Color_t)*CANVAS_MEMORY_SIZE);

    //long *memory = canvas;
    //do
    //{
    //    *memory = color;
    //    memory++;
    //} while (memory != canvasEnd);
}

void Canvas::PutPixel
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

    m_canvas[XY_TO_LINEAR(x,y)] = color;
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

    return m_canvas[XY_TO_LINEAR(x,y)];
}


inline Color_t * Canvas :: GetCanvas()
{
    return m_canvas;
}
