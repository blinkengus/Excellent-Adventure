#include <TimerOne.h>
#include "EffectManager.h"


EffectManager::EffectManager(long periodMicroSeconds, char canvasWidth, char canvasHeight)
{
    canvas0.Init(canvasWidth, canvasHeight);
    canvas1.Init(canvasWidth, canvasHeight);
    this->period = periodMicroSeconds;
    SetMode(EM_MODE_IDLE);
}

EffectManager::~EffectManager()
{
    Destroy();
}
void EffectManager::Destroy()
{
    canvas0.Destroy();
    canvas1.Destroy();
}

void EffectManager::AddEffectsArrays(Effect *effectsIdle, char sizeIdle,
                                     Effect *effectsRing, char sizeRing,
                                     Effect *effectsCall, char sizeCall)
{
    this->effectsIdle = effectsIdle;
    this->effectsCall = effectsCall;
    this->effectsRing = effectsRing;

    this->sizeIdle = sizeIdle;
    this->sizeRing = sizeRing;
    this->sizeCall = sizeCall;
}

void EffectManager::SetMode(char mode)
{
    this->mode = mode;
}

EffectManager *emGlobal;

void ISRGlobal()
{
    emGlobal->Callback();
}


void EffectManager::InstallAnimator()
{
    this->canvas0.InitPanels(10);
    
    emGlobal = this;
    
    //Timer1.initialize(period);
    //Timer1.attachInterrupt(ISRGlobal);
}

void EffectManager::Callback()
{
    this->canvas0.BlitToPanels();
    Effect *e = this->effectsIdle;
    e[0].func(&canvas0, EFFECTMODE_LOOP);
}

