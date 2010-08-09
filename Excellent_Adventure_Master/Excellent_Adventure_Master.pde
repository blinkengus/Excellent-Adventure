/*
    ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______
   /\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \
   \ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/
    \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\
     \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/

   ---What?---
   Combined "master" firmware for Excellent Adventure.
   Controls LED panels, dumb LEDs, SLIC, and takes analog
   sound and ambient light inputs.

   ---Why?---
   www.carbonworkshop.com/bm10

   ---Who?---
   Chris Condrat  - chris@g6net.com
   Gustavo Huber  - gush@carbonworkshop.com
   Daniel Massey  - pichiste@gmail.com
   Ryan Alexander - ryan@onecm.com

   ---When?---
   June 11, 2010
 */

#include <Wire.h>
#include <WProgram.h>
#include <TimerOne.h>
#include "EffectManager.h"
#include "Slic.h"
#include "Effects.h"


// Define if this is the master booth
#define BOOTH_MASTER

//
#define POLLING_DELAY       1000
                            
#define PERIOD_MICROSEC     50000
#define PANELS_WIDTH        9
#define PANELS_HEIGHT       10
                            
#define STATE_IDLE          0
#define STATE_RING          1
#define STATE_CALL          2
                            
#define PIN_SENSOR_LIGHT    0


//          +--------+   +--------+
//  Tip ----| SLIC_L |   | SLIC_L |
//          |        |   |        |
//  Ring ---|        |   |        |
//          +--------+   +--------+
//            |    |       |    |
//          +---------------------+
//          |                     |
//          |                     |
//          |                     |
//          |                     |
//          |                     |
//          +---------------------+

// L = Local; R = Remote

//pin definitions
//Analog 0: LIGHTSENSOR
//Analog 1: SL1_VOUT
//Analog 2: SL2_VOUT
//Analog 3:
//Analog 4: Remapped to I2C SDA for LED Panels
//Analog 5: Remapped to I2C SCK for LED Panels
//
//AREF:
//GND:
//D 13: SLI1+2 Power down
const byte SH2 = 12; //D 12: SL2_SHK
//D 11 (PWM): M4
//D 10 (PWM): M3
//D 9 (PWM): M2
const byte FR2 = 8;  //D 8: SL2_FR
//
const byte RM2 = 7;  //D 7: SL2_RM
//D 6 (PWM): M1
//D 5 (PWM):
const byte RM1 = 4;  //D 4: SL1_RM
const byte FR1 = 3;  //D 3 (PWM): SL1_FR
const byte SH1 = 2;  //D 2: SL1_SHK
//D 1: Arduino_TX
//D 0: Arduino_RX

int time0;

int blinkTime0;
bool blinkMode;

char state;

EffectManager EM(PERIOD_MICROSEC);
SLICControl SC;


Effect effects[1] =
{
    {&Spotlight, 0}
};


void setup()
{
    state = STATE_IDLE;
    
    // Pin assignments
    
    pinMode(SH2, INPUT);
    blinkMode = false;
    
    pinMode(13, OUTPUT);
    Serial.begin(38400);
    EM.AddEffectsArrays
    (
        effects, 1,
        effects, 1,
        effects, 1
    );
    EM.SetMode(EM_MODE_IDLE);
    EM.InitPanels();
    time0 = blinkTime0 = millis();
    //EM.InstallAnimator();
}

// Both on hook -> enable ringing
// OffHook -> Ring other phone -> OffHook Other phone -> Ring off
// -> Hang-up phone -> wait for other hang-up

void loop()
{
    switch (state){
        case STATE_IDLE:
            break;
        case STATE_RING:
            break;
        case STATE_CALL:
        default:
            break;
    };
    int time1 = millis();
    if((time1 - time0) > 20)
    {
        time0 = time1;
        EM.Callback();
    }
    
    if ((time1 - blinkTime0) > 1000)
    {
        blinkTime0 = time1;
        blinkMode = !blinkMode;
        digitalWrite(13, blinkMode ? HIGH : LOW);
    }

    //delay(POLLING_DELAY);
}