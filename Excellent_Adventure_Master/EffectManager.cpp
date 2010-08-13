#include <TimerOne.h>
#include "EffectManager.h"


EffectManager::EffectManager(long periodMicroSeconds)
{
    m_period = periodMicroSeconds;
    SetMode(EM_MODE_IDLE);
}


EffectManager::~EffectManager()
{
    Destroy();
}
void EffectManager::Destroy ()
{
    m_canvas0.Destroy();
    m_canvas1.Destroy();
}

void EffectManager::InitPanels()
{
    m_canvas0.InitPanels();
}


void EffectManager::AddEffectsArrays(Effect *effectsIdle, char sizeIdle,
                                     Effect *effectsRing, char sizeRing,
                                     Effect *effectsCall, char sizeCall)
{
    m_effectsIdle = effectsIdle;
    m_effectsCall = effectsCall;
    m_effectsRing = effectsRing;

    m_sizeIdle = sizeIdle;
    m_sizeRing = sizeRing;
    m_sizeCall = sizeCall;
}


void EffectManager::SetMode(char mode)
{
    m_mode = mode;
}


#ifdef ISR_ANIMATOR

EffectManager *emGlobal;

void ISRGlobal()
{
    emGlobal->Callback();
}


void EffectManager::InstallAnimator()
{
    Timer1.initialize(m_period);
    //typedef void (EffectManager ::* EMISR)();
    //EMISR p = &EffectManager::ISR;
    emGlobal = this;
    Timer1.attachInterrupt(ISRGlobal);
}

#endif


void EffectManager::Callback()
{
    this->m_canvas0.BlitToPanels();
    Effect * e = this->m_effectsIdle;
    e[0].func(&this->m_canvas0, EFFECTMODE_LOOP);
}
