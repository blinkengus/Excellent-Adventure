#ifndef _GLOBALS_H
#define _GLOBALS_H

// What modes we want to have applied to the entire project.

#define USE_ARDUINO
//#define USE_MAPLE

#define USE_UART



#define I2C_RATE                100000
#define SERIAL_RATE             38400

#define SPECTRUM_REDUCTION_FACTOR       1

//#define MEMORY_DYNAMIC
//#define BENCHMARK

// If we're connected to a MSGEQ7 spectrum analysis chip, otherwise use the
// software spectrum analysis.
#define SPECTRUM_USE_HARDWARE



/////////////////////////////////////////////////////////////////////////////
//
// ARDUINO DEFINES

#ifdef USE_ARDUINO
/////////////////////////////////////////////////////////////////////////////

#ifdef USE_UART
#define SERIAL                  (Serial)
#define PRINTLN(STR)            Serial.println(STR)
#define PRINT(STR)              Serial.print(STR)
#define SERIAL_WRITE(DATA,LEN)  Serial.write((DATA),(LEN))
#endif

// PIN DEFINITIONS:

// Analog 1 = Pin 15
#define BOOTH_PIN_LIGHT_SENSOR  1
#define BOOTH_PIN_LASER         11
#define BOOTH_PIN_LOW_POWER_EN  13
#define PHONE_PIN_RING_DETECT   10

#define SPECTRUM_PIN_STROBE     2
#define SPECTRUM_PIN_RESET      3

// ANALOG 0 = Pin 14
#define SPECTRUM_PIN_ANALOG     0

// Ring Mode
// Bias conditions during ringing. Set to 1 during ringing, 0 other.
#define SLIC_PIN_RING_MODE_L    7
#define SLIC_PIN_RING_MODE_R    4


// Forward/Reverse
// Logic 0 reverses Tip and Ring.  Toggled to produce ring output.

// NOT USED:
//#define SLIC_PIN_FORWARD_REVERSE_L  3
//#define SLIC_PIN_FORWARD_REVERSE_R  8

// Switch-hook
// Indicates off-hook condition when 1.
#define SLIC_PIN_SWITCH_HOOK_L  12
#define SLIC_PIN_SWITCH_HOOK_R  6


#define SLIC_PIN_RINGER_L       8
#define SLIC_PIN_RINGER_R       5

/////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// MAPLE DEFINES

#ifdef USE_MAPLE
/////////////////////////////////////////////////////////////////////////////


#ifdef USE_UART
#define SERIAL                  (Serial2)
#define PRINTLN(STR)            Serial2.println(STR)
#define PRINT(STR)              Serial2.print(STR)
#define SERIAL_WRITE(DATA,LEN)  Serial2.write((DATA),(LEN))
#endif

// Analog 1 = Pin 15
#define BOOTH_PIN_LIGHT_SENSOR  1
#define BOOTH_PIN_LASER         11
#define BOOTH_PIN_LOW_POWER_EN  13
#define PHONE_PIN_RING_DETECT   10

#define SPECTRUM_PIN_STROBE     2
#define SPECTRUM_PIN_RESET      3

// ANALOG 0 = Pin 14
#define SPECTRUM_PIN_ANALOG     0

// Ring Mode
// Bias conditions during ringing. Set to 1 during ringing, 0 other.
#define SLIC_PIN_RING_MODE_L    7
#define SLIC_PIN_RING_MODE_R    4


// Forward/Reverse
// Logic 0 reverses Tip and Ring.  Toggled to produce ring output.

// NOT USED:
//#define SLIC_PIN_FORWARD_REVERSE_L  3
//#define SLIC_PIN_FORWARD_REVERSE_R  8

// Switch-hook
// Indicates off-hook condition when 1.
#define SLIC_PIN_SWITCH_HOOK_L  12
#define SLIC_PIN_SWITCH_HOOK_R  6


#define SLIC_PIN_RINGER_L       8
#define SLIC_PIN_RINGER_R       5


// Are we using an ISR animator for the effect manager?  Does not work at all
// on the Arduino
// #define ISR_ANIMATOR

/////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////


#ifndef USE_UART
#define SERIAL
#define PRINTLN(STR)
#define PRINT(STR)
#define SERIAL_WRITE(DATA,LEN)
#endif

// Globals
#endif


