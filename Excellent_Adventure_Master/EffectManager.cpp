#include "Globals.h"

#ifdef ISR_ANIMATOR
#include <TimerOne.h>
#endif
#include <WProgram.h>
#include "EffectManager.h"
//#include "TimerOne.h"


#ifdef EM_DEBUG_ENABLE
#define EM_DEBUG(MSG)  PRINTLN(MSG)
#define EM_DEBUG2(MSG) PRINT(MSG)
#else
#define EM_DEBUG(MSG)
#define EM_DEBUG2(MSG) PRINT(MSG)
#endif

#define SET_EFFECT(EFFECT,CURRENT_EFFECT)                                   \
    runEffects = true;
//    effects = (EFFECT);                                                     \
//    currentEffect = (CURRENT_EFFECT);                                       \


//#define EM_DEBUG_NOBLIT

EffectManager :: EffectManager
(
    PowerManagement                             *   pm
)
{
    m_laserOn = false;
    m_pollLaser = m_pollDelay = millis();
    m_pm = pm;
    m_modePrevious = -1;
    m_currentIdle = 0;
    m_currentRing = 0;
    m_currentCall = 0;
    m_currentOver = 0;
    m_disablePanels = false;
    m_panelsDisabled = false;
    m_rebooting = false;
    m_rebootingPowerUp = false;
    m_duration = 0;
    SetMode(EM_MODE_IDLE);
}


EffectManager :: ~EffectManager()
{
    Destroy();
}
void EffectManager :: Destroy ()
{
    m_canvas.Destroy();
}

void EffectManager :: InitPanels()
{
    m_canvas.InitPanels();
}

void EffectManager :: InitPins()
{
    pinMode(PHONE_PIN_RING_DETECT, INPUT);
    pinMode(BOOTH_PIN_LASER, OUTPUT);


    digitalWrite(BOOTH_PIN_LASER, LASER_SIGNAL_DISABLE);
}


void EffectManager :: InitHardware()
{
    InitPanels();
    InitSpectrum();
    InitPins();
}

void EffectManager :: InitSpectrum()
{
    m_spectrum.InitSpectrumPins();
}

void EffectManager :: LaserOn()
{
    if (!m_panelsDisabled)
    {
        m_laserOn = true;
        EM_DEBUG("EM:   Laser ON");
    }
}

void EffectManager :: LaserOff()
{
    m_laserOn = false;
    EM_DEBUG("EM:   Laser OFF");
    digitalWrite(BOOTH_PIN_LASER, LASER_SIGNAL_DISABLE);
}

void EffectManager :: PollLaser
(
    unsigned long                                   time
)
{
    if (m_laserOn)
    {
        static bool laserMode = false;
        if (laserMode)
        {
            if ((time - m_pollLaser) > LASER_TIME_ON_MS)
            {
                digitalWrite(BOOTH_PIN_LASER, LASER_SIGNAL_DISABLE);
                m_pollLaser = time;
                laserMode = false;
            }
        } else {
            if ((time - m_pollLaser) > LASER_TIME_OFF_MS)
            {
                digitalWrite(BOOTH_PIN_LASER, LASER_SIGNAL_ENABLE);
                m_pollLaser = time;
                laserMode = true;
            }
        }
    }
}

void EffectManager :: AddEffectsArrays
( 
    Effect                                      *   effectsIdle,
    char                                            sizeIdle,
    Effect                                      *   effectsRing,
    char                                            sizeRing,
    Effect                                      *   effectsCall,
    char                                            sizeCall,
    Effect                                      *   effectsOver,
    char                                            sizeOver
)
{
    m_effectsIdle = effectsIdle;
    m_effectsCall = effectsCall;
    m_effectsRing = effectsRing;
    m_effectsOver = effectsOver;

    m_sizeIdle = sizeIdle;
    m_sizeRing = sizeRing;
    m_sizeCall = sizeCall;
    m_sizeOver = sizeOver;
}

void EffectManager :: SetMode
(
    char                                            mode
)
{
    m_mode = mode;
}


void EffectManager :: EnableEffects()
{
    EM_DEBUG("EM:   Enable Effects - LEAVING LOW POWER MODE");
    m_disablePanels = false;
}

void EffectManager :: DisableEffects()
{
    EM_DEBUG("EM:   Disable Effects - ENTERING LOW POWER MODE");
    m_disablePanels = true;
}

bool EffectManager :: EffectsDisabled()
{
    // We are only actually disabled 
    return (m_mode == EM_MODE_DISABLE_STANDBY);
}

unsigned short * EffectManager :: GetSpectrum()
{
    return m_spectrum.GetSpectrum();
}

unsigned short EffectManager :: GetRandomNumber()
{
    return random(0xFFFF);
}

unsigned short EffectManager :: GetRandom(unsigned short max)
{
    return random(max);
}

// This just sets up the reboot mode and an initial reboot.  You must then
// repeatedly check the status of the reboot.
void EffectManager :: RebootPanels()
{
    EM_DEBUG("EM:   Rebooting panels... powering down.");
    m_rebooting = true;
    m_rebootingPowerUp = false;
    m_pm->PowerDown();
}

bool EffectManager :: RebootComplete()
{
    
    if (m_rebooting)
    {
        if (m_pm->Ready())
        {
            if (!m_rebootingPowerUp)
            {
                EM_DEBUG("EM:   Rebooting panels... powering up.");
                m_rebootingPowerUp = true;
                m_pm->PowerUp();
            } else {           
                EM_DEBUG("EM:   Reboot complete!");
                // Idea: wait another bit before switching mode?
                m_rebooting = false;
            }
        }
        return false;
    } 
    return true;
}


void EffectManager :: Poll
(
    unsigned long                                   time,
    bool                                            offHook
)
{
    static bool runEffects = false;
    static unsigned short effectCount = 0;
    static bool laserMode = false;
    PollLaser(time);        
    if ((time - m_pollDelay) > EFFECT_POLL_DELAY_MS)
    {
        //EM_DEBUG("BEGIN POLL");
        //EM_DEBUG(effectCount);
        //static int ringer = 0;
        //ringer++;
        
        m_pollDelay = time;
        bool switchEffect = (effectCount == 0);

        // Which index are we in the array of effects for this mode:
        //static char *currentEffect; // 0 in case it is not specified

        //static Effect * effects;
        switch (m_mode)
        {
        case EM_MODE_RING:
        case EM_MODE_IDLE:
            if
            (
                digitalRead(PHONE_PIN_RING_DETECT)
            //||  (ringer & 32)
            )
            {
                SetMode(EM_MODE_RING);
                if (switchEffect)
                {
                    m_currentRing++;
                    if (m_currentRing >= m_sizeRing)
                    {
                        m_currentRing = 0;
                    }
                    effectCount = m_effectsRing[m_currentRing].duration;
                }
            } else {
                SetMode(EM_MODE_IDLE);
                if (switchEffect)
                {
                    m_currentIdle++;
                    if (m_currentIdle >= m_sizeIdle)
                    {
                        m_currentIdle = 0;
                    }
                    effectCount = m_effectsIdle[m_currentIdle].duration;
                }
            }
            
            if (offHook)
            {
                SetMode(EM_MODE_CALL);
            }
            
            if (m_disablePanels)
            {
                SetMode(EM_MODE_DISABLE);
                m_panelsDisabled = false;
            }
            
            break;
        case EM_MODE_CALL:
            if (m_laserCallCount == 0)
            {
                LaserOff();
                m_laserCallCount = -1;
            }
            else if (m_laserCallCount > 0)
            {
                m_laserCallCount--;
            }
                
            if (switchEffect)
            {
                m_currentCall++;
                if (m_currentCall >= m_sizeCall)
                {
                    m_currentCall = 0;
                }
                effectCount = m_effectsCall[m_currentCall].duration;

            }
            //currentEffect = m_currentCall;
            if (!offHook)
            {
                SetMode(EM_MODE_CALLENDED);
            }
            if ((time - m_duration) > MAX_CALL_DURATION_MS)
            {
                EM_DEBUG("EM:   Call went overtime.");
                SetMode(EM_MODE_CALL_OVERTIME);
            }
            break;
        case EM_MODE_CALL_OVERTIME:
            // Whoops, the person was on the line too long:
            if (switchEffect)
            {
                m_currentOver++;
                if (m_currentOver >= m_sizeOver)
                {
                    m_currentOver = 0;
                }
                effectCount = m_effectsOver[m_currentOver].duration;
            }            
            if (!offHook)
            {
                SetMode(EM_MODE_CALLENDED);
            }
            break;
        case EM_MODE_CALLENDED_REBOOTED:
            if (RebootComplete())
            {
                SetMode(EM_MODE_IDLE);
            }
            break;
        case EM_MODE_DISABLE_STANDBY:
            if (m_disablePanels == false)
            {
                m_panelsDisabled = false;
                RebootPanels();
                SetMode(EM_MODE_CALLENDED_REBOOTED);
            }
            
            // Do nothing.
            m_pollDelay += EM_FADE_DELAY_MS; // To reduce poll time.

            break;
        default:
            break;

        }

        if (m_modePrevious != m_mode)
        {
            m_modePrevious = m_mode;

            // All variables in this section must be persistent
            runEffects = false;
            // Triggered once per mode change.
            switch(m_mode)
            {
            case EM_MODE_IDLE:
                EM_DEBUG("EM:   State: IDLE");
                LaserOff();
                SET_EFFECT(m_effectsIdle, &m_currentIdle);
                break;
            case EM_MODE_RING:
                EM_DEBUG("EM:   State: RING");
                m_canvas.ClearCeiling(0);
                LaserOn();
                m_currentRing++;
                if (m_currentRing >= m_sizeRing)
                {
                    m_currentRing = 0;
                }
                effectCount = m_effectsRing[m_currentRing].duration;

                SET_EFFECT(m_effectsRing, &m_currentRing);
                break;
            case EM_MODE_CALL:
                EM_DEBUG("EM:   State: CALL");
                // The current time so we can shut down the effects if the
                // person is taking too long
                m_duration = time;
                effectCount = 0;
                LaserOn();
                m_laserCallCount = EM_LASER_CALL_COUNT;
                // Restore the lights!
                m_canvas.ClearCeiling(0);
                SET_EFFECT(m_effectsCall, &m_currentCall);
                break;               
            case EM_MODE_CALL_OVERTIME:
                EM_DEBUG("EM:   State: OVER");
                LaserOff();
                // The current time so we can shut down the effects if the
                // person is taking too long
                effectCount = 0;
                // Restore the lights!
                SET_EFFECT(m_effectsOver, &m_currentOver);
                break;             
            case EM_MODE_CALLENDED_REBOOTED:
                EM_DEBUG("EM:   State: CALLENDED_REBOOTED");
                break;  
            case EM_MODE_CALLENDED:
                EM_DEBUG("EM:   State: CALLENDED");
                LaserOff();
                // The call ended, fade those panels out.
                m_canvas.FadeToBlack();

                //EM_DEBUG2("EM:   State: CALLENDED: time: ");
                //EM_DEBUG((unsigned long)time);
                //EM_DEBUG2("EM:   State: CALLENDED: poll: ");
                //EM_DEBUG((unsigned long)m_pollDelay);

                m_pollDelay += EM_FADE_DELAY_MS;
                //EM_DEBUG2("EM:   State: CALLENDED: poll: ");
                //EM_DEBUG((unsigned long)m_pollDelay);
                //EM_DEBUG("EM:   State: CALLENDED (fading command issued)");
                
                SetMode(EM_MODE_CALLENDED_FADE_END);
                //EM_DEBUG("EM:   State: CALLENDED - FINISH");
                break;
            case EM_MODE_CALLENDED_FADE_END:
                EM_DEBUG("EM:   State: CALLENDED FADEEND");
                // Wait until the power manager is ready, and then
                // reboot the panels:
                RebootPanels();
                SetMode(EM_MODE_CALLENDED_REBOOTED);
                /*if (m_pm->Ready())
                {
                    if (m_pm->GetPowerStatus() == PM_POWER_ON)
                    {
                        m_pm->PowerDown();
                    } else {
                        m_pm->PowerUp();
                        // Idea: wait another bit before switching mode?
                        SetMode(EM_MODE_IDLE);
                    }
                }*/
                break;
            case EM_MODE_DISABLE:
                EM_DEBUG("EM:   State: DISABLE - Fading panels");
                LaserOff();
                // Fade out
                m_canvas.FadeToBlack();
                m_pollDelay += EM_FADE_DELAY_MS; // 2 seconds delay
                SetMode(EM_MODE_DISABLE_FADE_END);
                break;
            case EM_MODE_DISABLE_FADE_END:
                EM_DEBUG("EM:   State: DISABLE_FADEEND");
                // We're all faded out, terminate power.  The only way out of 
                // this mode is a call to EnableEffects().
                m_pm->PowerDown();
                SetMode(EM_MODE_DISABLE_STANDBY);
                break;
            case EM_MODE_DISABLE_STANDBY:
                EM_DEBUG("EM:   State: DISABLE_STANDBY - Low power mode activated.");
                break;
            default:
                EM_DEBUG("EM:   State: UNKNOWN!");
                break;
            };
        }
        if (runEffects)
        {
            m_spectrum.ReadSpectrum();
            switch(m_mode)
            {
            case EM_MODE_IDLE:
                m_effectsIdle[m_currentIdle].func
                (
                    &m_canvas, 
                    const_cast<EffectManager *>(this), 
                    EFFECTMODE_LOOP
                );
                break;
            case EM_MODE_RING:
                m_effectsRing[m_currentRing].func
                (
                    &m_canvas, 
                    const_cast<EffectManager *>(this), 
                    EFFECTMODE_LOOP
                );
                break;
            case EM_MODE_CALL:
                m_effectsCall[m_currentCall].func
                (
                    &m_canvas, 
                    const_cast<EffectManager *>(this), 
                    EFFECTMODE_LOOP
                );
                break;
            case EM_MODE_CALL_OVERTIME:
                m_effectsOver[m_currentOver].func
                (
                    &m_canvas, 
                    const_cast<EffectManager *>(this), 
                    EFFECTMODE_LOOP
                );
                break;
            default:
                EM_DEBUG("EM:   ERROR MODE");
                break;
            }
            
            //EM_DEBUG("runEffects");
            //EM_DEBUG((int)(*currentEffect));
            //EM_DEBUG("ringer");
            //EM_DEBUG((int)(ringer));
            //Effect *theEffect = (effects + (*currentEffect));
            
#ifndef EM_DEBUG_NOBLIT
            m_canvas.BlitToPanels();
#endif
        }
        if (effectCount > 0)
        {
            effectCount--;
        }

    }
}




#ifdef ISR_ANIMATOR


EffectManager *emGlobal;

void ISRGlobal()
{
    emGlobal->Poll();
}

void EffectManager :: InstallAnimator ()
{
    // microseconds
    Timer1.initialize(EFFECT_POLL_DELAY_MS * 1000);
    //typedef void (EffectManager ::* EMISR)();
    //EMISR p = &EffectManager::ISR;
    emGlobal = this;
    Timer1.attachInterrupt(ISRGlobal);
}

#endif


