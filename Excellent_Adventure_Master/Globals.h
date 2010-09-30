#ifndef _GLOBALS_H
#define _GLOBALS_H

// What modes we want to have applied to the entire project.

#define USE_ARDUINO
//#define USE_MAPLE


//#define USE_UART

// You must enable the UART to use the simulator.  Using the simulator will
// disable the I2C panel communications.
//#define USE_PANEL_SIMULATOR

//#define CEILING_LIGHTS_DISABLE

//#define PM_DISABLE


// DEBUG enabling:

// Enable debug messages for the power management
//#define PM_DEBUG_ENABLE


// Enable debug messages for the effect manager
//#define EM_DEBUG_ENABLE

// Turns off the blittin (will not show anything on panels if you enable
// this):
//#define EM_DEBUG_NOBLIT


// Show messages in the SLIC:
//#define SC_DEBUG_ENABLE

#define PM_POWER_UP_DELAY_MS            0

#define WATCHDOG_PERIOD_MS              8192

// Force the values of the SLIC offhooks:

//#define SLIC_FORCE_LOCAL_OFFHOOK
//#define SLIC_FORCE_REMOTE_OFFHOOK

//#define SLIC_FORCE_LOCAL_ONHOOK
//#define SLIC_FORCE_REMOTE_ONHOOK

// This will make the local offhook alternate from offhook to onhook about
// every 5-10 seconds
//#define SLIC_DEBUG_OFFON_LOCAL

#define EM_LASER_CALL_COUNT             1024


#define LASER_TIME_ON_MS                7000
#define LASER_TIME_OFF_MS               128


#define I2C_RATE                        100000
#define SERIAL_RATE                     38400

#define SPECTRUM_REDUCTION_FACTOR       1


#define LIGHT_SENSOR_THRESHOLD          512
#define LIGHT_SENSOR_POLLING_MS         1024

#define DEBOUNCE_ALL_ONES               0xFFFF
#define DEBOUNCE_ALL_ZEROS              0x0000
#define DEBOUNCE_UNDEFINED              0xAAAA


//#define MEMORY_DYNAMIC
//#define BENCHMARK

// If we're connected to a MSGEQ7 spectrum analysis chip, otherwise use the
// software spectrum analysis.
#define SPECTRUM_USE_HARDWARE

// Blinks an LED on the Arduino to indicate operation
//#define BLINK_ENABLED

// Define if this is the master booth
#define BOOTH_MASTER

// Minimum MS to wait before next frame of the effects:
#define EFFECT_POLL_DELAY_MS            20
#define BLINK_MS                        1000

// Seconds
#define MAX_CALL_DURATION_SEC           (5*60)
#define MAX_CALL_DURATION_MS            (MAX_CALL_DURATION_SEC*1000)

#define CALLEND_WAIT_MS                 3072


// DEFINES

#define PM_LOW_POWER_MODE_ON            true
#define PM_LOW_POWER_MODE_OFF           false
#define PM_POWER_OFF                    false
#define PM_POWER_ON                     true

#define PM_LOW_POWER_SIGNAL_ENABLE      LOW
#define PM_LOW_POWER_SIGNAL_DISABLE     HIGH

#define LASER_SIGNAL_ENABLE             HIGH
#define LASER_SIGNAL_DISABLE            LOW




// When fading to black, how long to wait until we power down:
#define EM_FADE_DELAY_MS                2048

/////////////////////////////////////////////////////////////////////////////
//
// ARDUINO DEFINES

#ifdef USE_ARDUINO
/////////////////////////////////////////////////////////////////////////////

#ifdef USE_UART
#define SERIAL                          (Serial)
#define PRINTLN(STR)                    Serial.println(STR)
#define PRINT(STR)                      Serial.print(STR)
#define SERIAL_WRITE(DATA,LEN)          Serial.write((DATA),(LEN))
#endif

// PIN DEFINITIONS:

// Analog 1 = Pin 15
#define BOOTH_PIN_LIGHT_SENSOR          1
#define BOOTH_PIN_LASER                 11
#define BOOTH_PIN_LOW_POWER_EN          13
#define PHONE_PIN_RING_DETECT           10

#define SPECTRUM_PIN_STROBE             2
#define SPECTRUM_PIN_RESET              3

#define BLINK_PIN                       13

#define WATCHDOG_PULSE_PIN              9


// ANALOG 0 = Pin 14
#define SPECTRUM_PIN_ANALOG             0

// Ring Mode
// Bias conditions during ringing. Set to 1 during ringing, 0 other.
#define SLIC_PIN_RING_MODE_L            7
#define SLIC_PIN_RING_MODE_R            4


// Forward/Reverse
// Logic 0 reverses Tip and Ring.  Toggled to produce ring output.

// NOT USED:
//#define SLIC_PIN_FORWARD_REVERSE_L  3
//#define SLIC_PIN_FORWARD_REVERSE_R  8

// Switch-hook
// Indicates off-hook condition when 1.
#define SLIC_PIN_SWITCH_HOOK_L          12
#define SLIC_PIN_SWITCH_HOOK_R          6


#define SLIC_PIN_RINGER_L               8
#define SLIC_PIN_RINGER_R               5

/////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// MAPLE DEFINES

#ifdef USE_MAPLE
/////////////////////////////////////////////////////////////////////////////


#ifdef USE_UART
#define SERIAL                          (Serial2)
#define PRINTLN(STR)                    Serial2.println(STR)
#define PRINT(STR)                      Serial2.print(STR)
#define SERIAL_WRITE(DATA,LEN)          Serial2.write((DATA),(LEN))
#endif

// Analog 1 = Pin 15
#define BOOTH_PIN_LIGHT_SENSOR          1
#define BOOTH_PIN_LASER                 11
#define BOOTH_PIN_LOW_POWER_EN          13
#define PHONE_PIN_RING_DETECT           10

#define SPECTRUM_PIN_STROBE             2
#define SPECTRUM_PIN_RESET              3

#define BLINK_PIN                       13


// ANALOG 0 = Pin 14
#define SPECTRUM_PIN_ANALOG             0

// Ring Mode
// Bias conditions during ringing. Set to 1 during ringing, 0 other.
#define SLIC_PIN_RING_MODE_L            7
#define SLIC_PIN_RING_MODE_R            4


// Forward/Reverse
// Logic 0 reverses Tip and Ring.  Toggled to produce ring output.

// NOT USED:
//#define SLIC_PIN_FORWARD_REVERSE_L  3
//#define SLIC_PIN_FORWARD_REVERSE_R  8

// Switch-hook
// Indicates off-hook condition when 1.
#define SLIC_PIN_SWITCH_HOOK_L          12
#define SLIC_PIN_SWITCH_HOOK_R          6


#define SLIC_PIN_RINGER_L               8
#define SLIC_PIN_RINGER_R               5


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
