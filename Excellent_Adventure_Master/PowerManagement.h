#ifndef _POWER_MANAGEMENT_H
#define _POWER_MANAGEMENT_H
#include "Globals.h"

class PowerManagement
{
    bool                                            m_powerStatus;
    bool                                            m_lowPowerStatus;
    bool                                            m_lowPowerStatusOld;
    short                                           m_debounce;
    unsigned long                                   m_time;
    bool                                            m_ready;
public:
    PowerManagement();
    ~PowerManagement();
    void InitPins();
    bool Ready();
    bool GetPowerStatus();
    bool GetLowPowerStatus();
    void PowerUp();
    void PowerDown();
    // Returns whether the status has changed:
    bool Poll
    (
        unsigned long                               time
    );

};


#endif
