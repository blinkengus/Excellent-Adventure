#ifndef _EFFECT_MANAGER_H
#define _EFFECT_MANAGER_H
#include "Canvas.h"

#define EM_MODE_IDLE                0
#define EM_MODE_RING                1
#define EM_MODE_CALL                2

#define EFFECTMODE_INTRO            0
#define EFFECTMODE_LOOP             1
#define EFFECTMODE_OUTRO            2


// Idle -> Ring -> 


// The effect manager (EM) controls the effect scheduling for each booth.  It 
// is driven by a timer interrupt service routine (ISR), and switches between
// various effects based on different conditions.

struct Effect
{
    int                                             (*func)(Canvas *, char);
    int                                             flags;
};




class EffectManager
{
    Effect                                          *   m_effectsIdle;
    Effect                                          *   m_effectsRing;
    Effect                                          *   m_effectsCall;

    char                                                m_sizeIdle;
    char                                                m_sizeRing;
    char                                                m_sizeCall;
    char                                                m_mode;
    long                                                m_period;
    
    Canvas                                              m_canvas0;
    Canvas                                              m_canvas1;

public:
    EffectManager
    (
        long                                            periodMicroSeconds

    );
    ~EffectManager();

    void Destroy();
    // An effect is a static function that is called for each frame of the
    // animation.  The "type" of an effect is its 

    void InitPanels();

    void AddEffectsArrays
    ( 
        Effect                                      *   effectsIdle,
        char                                            sizeIdle,
        Effect                                      *   effectsRing,
        char                                            sizeRing,
        Effect                                      *   effectsCall,
        char                                            sizeCall
    );

    void SetMode
    (
        char                                            mode
    );

    void InstallAnimator();

    void Callback();
    
};

//extern void ISRGlobal();


#endif
