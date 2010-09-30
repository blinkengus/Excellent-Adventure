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

#include <WProgram.h>
#ifdef USE_ARDUINO
#include <Wire.h>
#endif

#include "Globals.h"


//#define DEBUG
//#define WATCHDOG_ENABLED


#include "EffectManager.h"
#include "Slic.h"
#include "Effects.h"


//#define SERIAL_ENABLED



#define STATE_IDLE              0
#define STATE_RING              1
#define STATE_CALL              2
#define STATE_CALLENDED         3
#define STATE_BUSY              4





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
#include "WProgram.h"
void setup();
void loop();
void PhoneControl(bool offHookLocal, bool offHookRemote);
unsigned long time;
unsigned long effectTime0;
unsigned long callEndTime;

unsigned int blinkTime0;
bool blinkMode;
bool callEnded;

char state;
#ifdef DEBUG
char debugState;
#endif

#ifdef WATCHDOG_ENABLED
unsigned long watchdogTime0;
#endif
PowerManagement     PM;
EffectManager       EM(&PM);
SLICControl         SC;

void setup()
{
    state = STATE_IDLE;

    // Pin assignments

#ifdef USE_UART
    SERIAL.begin(SERIAL_RATE);
    PRINTLN("START");
#endif

    EM.AddEffectsArrays
    (
        effectsIdle,        EFFECTS_NUM_IDLE, // Effects array, length
        effectsRing,        EFFECTS_NUM_RING, 
        effectsCall,        EFFECTS_NUM_CALL,
        effectsOver,        EFFECTS_NUM_OVER
    );
    EM.SetMode(EM_MODE_IDLE);
    EM.InitHardware();
    SC.InitPins();
    PM.InitPins();

    time = millis();
   
#ifdef DEBUG
    debugState = -1;
#endif

#ifdef BLINK_ENABLED
    blinkMode = false;
    pinMode(BLINK_PIN, OUTPUT);
    blinkTime0 = time;
#endif

#ifdef WATCHDOG_ENABLED
    watchdogTime0 = time;
#endif
    //EM.InstallAnimator();
    EM.RebootPanels();

    while (!EM.RebootComplete())
    {
        PM.Poll(millis());
    }
}

// Both on hook -> enable ringing
// OffHook -> Ring other phone -> OffHook Other phone -> Ring off
// -> Hang-up phone -> wait for other hang-up 


void loop()
{
    time = millis();
    SC.Poll(time);
    // PollOffHooks returns 
    bool offHookRemote = SC.IsOffHookRemote();
    bool offHookLocal = SC.IsOffHookLocal();
    
    //bool offHookLocal = time & 2048;
    // When low power mode is enabled, notify the EM.  When the EM is done
    // with its shutdown routine, then outright kill the power.

    // Poll whether the power management low power status has changed and act
    // on it.
    if (PM.Poll(time))
    {
        if (PM.GetLowPowerStatus() == PM_LOW_POWER_MODE_ON)
        {
            EM.DisableEffects();
        } else {
            EM.EnableEffects();
        }
    }
    EM.Poll(time, offHookLocal);
    PhoneControl(offHookLocal, offHookRemote);

#ifdef WATCHDOG_ENABLED
    if ((time - watchdogTime0) > WATCHDOG_PERIOD_MS)
    {
        watchdogTime0 = time;
        PRINT("* time: ");
        PRINTLN((unsigned long) time);
    }
#endif
#ifdef BLINK_ENABLED
    if ((time - blinkTime0) > BLINK_MS)
    {
        blinkTime0 = time;
        blinkMode = !blinkMode;
        digitalWrite(BLINK_PIN, blinkMode ? HIGH : LOW);
    }    
#endif

    // Modulate Framerate a bit
    unsigned long dur = millis() - time;
    if(dur < 20)
        delay(20 - dur);
    
    // Output Random Value for WatchDog 'Duino
    digitalWrite(WATCHDOG_PULSE_PIN, ((time >> 8) & 0x1) == 1 ? HIGH : LOW);
}


void PhoneControl(bool offHookLocal, bool offHookRemote)
{


#ifdef DEBUG
    if (debugState != state)
    {
        
        switch (debugState)
        {
        case STATE_IDLE:
            PRINT("MAIN: State: IDLE");
            break;
        case STATE_RING:
            PRINT("MAIN: State: RING");
            break;
        case STATE_CALL:
            PRINT("MAIN: State: CALL");
            break;
        case STATE_CALLENDED:
            PRINT("MAIN: State: CALL ENDED");
            break;
        case -1:
            PRINT("MAIN: State: INIT");
            break;
        default:
            PRINT("MAIN: State: UNKNOWN val = ");
            PRINT((int)debugState);
            break;
        }

        PRINT(" -> ");
        switch (state)
        {
        case STATE_IDLE:
            PRINTLN("IDLE");
            break;
        case STATE_RING:
            PRINTLN("RING");
            break;
        case STATE_CALL:
            PRINTLN("CALL");
            break;
        case STATE_CALLENDED:
            PRINTLN("CALL ENDED");
            break;
        default:
            PRINT("UNKNOWN val = ");
            PRINTLN((int)state);
            break;
        }
        debugState = state;
    }
#endif

    switch (state)
    {
    case STATE_IDLE:
        if (offHookLocal || offHookRemote)
        {
            // Wait until one of the phones is off the hook.
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
            //callEnded = false;
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
        if (!offHookRemote && !offHookLocal)
        {
            // Both people hung up, end the call!
            callEndTime = time;
            state = STATE_CALLENDED;
        } 
        /*
        if (offHookLocal && offHookRemote)
        {
            // Call is in progress
            // If we want to resume the call:
            if (callEnded)
            {
                // If we resumed our call after one party hung up.
                //Audio.StopDialTone();
                callEnded = false;
            }
        } 
        else else {
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
        */
        break;
    case STATE_CALLENDED:
        if (offHookRemote || offHookLocal)
        {
            // What? Someone picked up again?
            state = STATE_CALL;
            
        }
        // Wait a little before resetting
        if ( (time - callEndTime) > CALLEND_WAIT_MS)
        {
            state = STATE_IDLE;
        }
        break;

    default:
        break;

    };
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

