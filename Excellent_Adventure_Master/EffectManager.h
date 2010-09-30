#ifndef _EFFECT_MANAGER_H
#define _EFFECT_MANAGER_H
#include "Globals.h"
#include "Spectrum.h"
#include "PowerManagement.h"
#include "Canvas.h"

#define EM_MODE_IDLE                0x00
#define EM_MODE_RING                0x01

// 16 + submodes
#define EM_MODE_CALL                0x10
#define EM_MODE_CALLENDED           0x11
// Overtime: when someone has been on the line too long
#define EM_MODE_CALL_OVERTIME       0x12
#define EM_MODE_CALLENDED_FADE_END  0x13
#define EM_MODE_CALLENDED_REBOOTED  0x14
// 64 + submodes
#define EM_MODE_DISABLE             0x40
#define EM_MODE_DISABLE_FADE_END    0x41
#define EM_MODE_DISABLE_STANDBY     0x42

#define EFFECTMODE_INTRO            0
#define EFFECTMODE_LOOP             1
#define EFFECTMODE_OUTRO            2

#define EM_CALL_DURATION_FRAMES     ((1000 / EFFECT_POLL_DELAY_MS) * MAX_CALL_DURATION_SEC)

// Idle -> Ring -> 


// The effect manager (EM) controls the effect scheduling for each booth.  It 
// is driven by a timer interrupt service routine (ISR), and switches between
// various effects based on different conditions.

class EffectManager;

struct Effect
{
    int                                             (*func)
                                                    (
                                                        Canvas *, 
                                                        EffectManager *,
                                                        char
                                                    );
    int                                             duration;
};




class EffectManager
{
    Effect                                      *   m_effectsIdle;
    Effect                                      *   m_effectsRing;
    Effect                                      *   m_effectsCall;
    Effect                                      *   m_effectsOver;

    bool                                            m_rebooting;
    bool                                            m_rebootingPowerUp;

    char                                            m_sizeIdle;
    char                                            m_currentIdle;
    char                                            m_sizeRing;
    char                                            m_currentRing;
    char                                            m_sizeCall;
    char                                            m_currentCall;
    char                                            m_sizeOver;
    char                                            m_currentOver;
    int                                             m_laserCallCount;
    // m_disablePanels tells the system to disable the panels.
    // m_panelsDisabled is the status
    bool                                            m_disablePanels;
    bool                                            m_panelsDisabled;

    bool                                            m_laserOn;
    char                                            m_mode;
    char                                            m_modePrevious;
    unsigned long                                   m_pollDelay;
    unsigned long                                   m_duration;
    unsigned long                                   m_pollLaser;
    Canvas                                          m_canvas;
    Spectrum                                        m_spectrum;
    PowerManagement                             *   m_pm;
    void InitPanels();
    void InitSpectrum();
    void InitPins();
public:
    EffectManager
    (
        PowerManagement                         *   pm
    );
    ~EffectManager();

    void Destroy();
    // An effect is a static function that is called for each frame of the
    // animation.  The "type" of an effect is its 

    void InitHardware();

    void AddEffectsArrays
    ( 
        Effect                                  *   effectsIdle,
        char                                        sizeIdle,
        Effect                                  *   effectsRing,
        char                                        sizeRing,
        Effect                                  *   effectsCall,
        char                                        sizeCall,
        Effect                                  *   effectsOver,
        char                                        sizeOver
    );

    void SetMode
    (
        char                                        mode
    );

    unsigned short GetRandomNumber();
    unsigned short GetRandom(unsigned short max);

    void InstallAnimator();
    unsigned short * GetSpectrum();

    // These two are power management calls, not to be used for rebooting
    // panels.
    void EnableEffects();
    void DisableEffects();
    bool EffectsDisabled();

    void RebootPanels();
    bool RebootComplete();
    // Note that this will be called during a ring, and will be auto-cut off
    // at the beginning of the next frame.

    void LaserOn();
    void LaserOff();
    void PollLaser
    (
        unsigned long                               time
    );
        
    void Poll
    (
        unsigned long                               time,
        bool                                        offHook
    );
    
};

//extern void ISRGlobal();


#endif
