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


#define DEBUG

#include "EffectManager.h"
#include "Slic.h"
#include "Effects.h"

// Blinks an LED on the Arduino to indicate operation
#define BLINK_ENABLED

#define SERIAL_ENABLED

// Define if this is the master booth
#define BOOTH_MASTER

//
#define POLLING_DELAY           1000

#define PERIOD_MICROSEC         50000
#define PANELS_WIDTH            9
#define PANELS_HEIGHT           10

#define STATE_IDLE              0
#define STATE_RING              1
#define STATE_CALL              2
#define STATE_CALLENDED         3
#define STATE_BUSY              4

#define PIN_SENSOR_LIGHT        0

#define MAX_CALL_DURATION_MS    (15*60*1000)

#define MAX_CALLEND_WAIT_MS     (2*1000)

#define EFFECT_CALLBACK_MS      20
#define BLINK_MS                1000

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

unsigned int time0;
unsigned long callTime;

unsigned int blinkTime0;
bool blinkMode;
bool callEnded;

char state;
#ifdef DEBUG
char debugState;
#endif

EffectManager EM(PERIOD_MICROSEC);
SLICControl SC;

Effect effects[] =
{
    {&Spotlight, 0}
,   {&SimpleColumns, 0}
};


void setup()
{
    state = STATE_IDLE;

    // Pin assignments

#ifdef SERIAL_ENABLED
    Serial.begin(38400);
#endif

    EM.AddEffectsArrays(
        effects, 1,
        effects, 1,
        effects, 1
    );
    EM.SetMode(EM_MODE_IDLE);
    EM.InitPanels();

    SC.InitSLICPins();

    time0 = millis();

#ifdef DEBUG
    debugState = -1;
#endif

#ifdef BLINK_ENABLED
    blinkMode = false;
    pinMode(13, OUTPUT);
    blinkTime0 = time0;
#endif
    //EM.InstallAnimator();
}

// Both on hook -> enable ringing
// OffHook -> Ring other phone -> OffHook Other phone -> Ring off
// -> Hang-up phone -> wait for other hang-up



void loop()
{
    // PollOffHooks returns
    unsigned long time1 = SC.Poll();
    bool offHookRemote = SC.IsOffHookRemote();
    bool offHookLocal = SC.IsOffHookLocal();

#ifdef DEBUG
    if (debugState != state)
    {
        debugState = state;
        switch (state)
        {
        case STATE_IDLE:
            Serial.println("State: IDLE");
            break;
        case STATE_RING:
            Serial.println("State: RING");
            break;
        case STATE_CALL:
            Serial.println("State: CALL");
            break;
        case STATE_CALLENDED:
            Serial.println("State: CALL ENDED");
            break;
        default:
            Serial.print("State: UNKNOWN val = ");
            Serial.println(state);
            break;
        }
    }
#endif

    switch (state)
    {
    case STATE_IDLE:
        if (offHookLocal || offHookRemote)
        {
            // Audio.PlayDialTone();
            state = STATE_RING;
            if (offHookLocal)
            {
                SC.StartRingingRemote();
            } else {
                SC.StartRingingLocal();
            }
        }
        break;
    case STATE_RING:
        if (offHookLocal && offHookRemote)
        {
            // Both phones picked up.  Start the call!
            // Audio.StopDialTone();
            state = STATE_CALL;
            SC.StopRingingAll();
            // Start the call timer.
            callEnded = false;

            callTime = time1;
        }
        else if (!offHookLocal && !offHookRemote)
        {
            // Hangup, both receivers were hung up.
            // Audio.StopDialTone();
            state = STATE_IDLE;
            SC.StopRingingAll();
        }
        // Else do nothing

        break;
    case STATE_CALL:
        if (offHookLocal && offHookRemote){
            // Call is in progress
            // If we want to resume the call:
            if (callEnded)
            {
                // If we resumed our call after one party hung up.
                //Audio.StopDialTone();
                callEnded = false;
            }
            if ((time1 - callTime) > MAX_CALL_DURATION_MS)
            {
                // Force a disconnection, i.e. cut the audio connection, and
                // inject our own message.
            }
        }
        else if (!offHookRemote && !offHookLocal){
            // Both people hung up, return to idle.
            callEnded = true;
            callTime = time1;
            // We use the same call time to delay the
            // circuit reset.
            state = STATE_CALLENDED;
        }
        else{
            // Someone hung up, but don't return to idle until the other
            // person did.
            //
            // Technically the call ended, should we allow the call to resume
            // if the receiver is picked up?
            callEnded = true;

            // Maybe wait a few seconds and allow someone to pick up?

            // If not, we need to also severe any audio transmission

            // Audio.SetTrack(CALL_DONE);
            // Audio.PlayDialTone();
        }
        break;
    case STATE_CALLENDED:
        if ((time1 - callTime) > MAX_CALLEND_WAIT_MS){
            state = STATE_IDLE;
        }
        break;

    default:
        break;

    };
    if ((time1 - time0) > EFFECT_CALLBACK_MS)
    {
        time0 = time1;
        EM.Callback();
    }
#ifdef BLINK_ENABLED
    if ((time1 - blinkTime0) > BLINK_MS){
        blinkTime0 = time1;
        blinkMode = !blinkMode;
        digitalWrite(13, blinkMode ? HIGH : LOW);
    }
#endif
    //delay(POLLING_DELAY);
}