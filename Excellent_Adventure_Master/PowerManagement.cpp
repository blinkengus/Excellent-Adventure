#include "PowerManagement.h"
#include "Globals.h"
#include <WProgram.h>

#ifdef PM_DEBUG_ENABLE
#define PM_DEBUG(MSG)  PRINTLN(MSG)
#define PM_DEBUG2(MSG) PRINT(MSG)
#else
#define PM_DEBUG(MSG)
#define PM_DEBUG2(MSG)
#endif


PowerManagement :: PowerManagement()
{
    m_time = millis();
    m_ready = true;
    m_lowPowerStatus = PM_LOW_POWER_MODE_OFF;
    m_lowPowerStatusOld = PM_LOW_POWER_MODE_OFF;
    m_powerStatus = PM_POWER_ON;
}

PowerManagement :: ~PowerManagement()
{
}

void PowerManagement :: InitPins()
{
    pinMode(BOOTH_PIN_LIGHT_SENSOR, INPUT);
    pinMode(BOOTH_PIN_LOW_POWER_EN, OUTPUT);
    digitalWrite(BOOTH_PIN_LOW_POWER_EN, PM_LOW_POWER_SIGNAL_DISABLE);
}

bool PowerManagement :: Ready()
{
    return m_ready;
}

bool PowerManagement :: GetPowerStatus()
{
    return m_powerStatus;
}

bool PowerManagement :: GetLowPowerStatus()
{
    return m_lowPowerStatus;
}

void PowerManagement :: PowerUp()
{
    PM_DEBUG("PM:   Powering Up");
    m_powerStatus = PM_POWER_ON;
#ifdef PM_DISABLE
    m_ready = true;
#else
    digitalWrite(BOOTH_PIN_LOW_POWER_EN, PM_LOW_POWER_SIGNAL_DISABLE);
    m_time = millis() + PM_POWER_UP_DELAY_MS;
    m_ready = false;
#endif
}

void PowerManagement :: PowerDown()
{
    PM_DEBUG("PM:   Powering Down");
    m_powerStatus = PM_POWER_OFF;
#ifdef PM_DISABLE
    m_ready = true;
#else
    digitalWrite(BOOTH_PIN_LOW_POWER_EN, PM_LOW_POWER_SIGNAL_ENABLE);
    m_time = millis();
    m_ready = false;
#endif
}

// The poll function checks the value of the light sensor.  The signal is
// debounced over a period of 16 * polling period -- about 16 seconds -- and
// has a threshold as well.
bool PowerManagement :: Poll
(
    unsigned long                                   time
)
{
#ifndef PM_DISABLE
    if ((time - m_time) > LIGHT_SENSOR_POLLING_MS)
    {
        m_time = time;

        // Shift the bitfield down one, and append the last value read.
        int lightSensor = analogRead(BOOTH_PIN_LIGHT_SENSOR);
        m_debounce = 
            (m_debounce << 1) 
            |   (   (lightSensor > LIGHT_SENSOR_THRESHOLD)
                    ?   0x1 
                    :   0
                );
        m_ready = true;
    }
    if 
    (   (m_debounce == DEBOUNCE_ALL_ZEROS) 
    ||  (m_debounce == DEBOUNCE_ALL_ONES)
    ) 
    {
        m_lowPowerStatus = 
            (m_debounce == DEBOUNCE_ALL_ONES) 
            ?   PM_LOW_POWER_MODE_ON
            :   PM_LOW_POWER_MODE_OFF;
        if (m_lowPowerStatusOld != m_lowPowerStatus)
        {
            m_lowPowerStatusOld = m_lowPowerStatus;
#ifdef PM_DEBUG_ENABLE
            PM_DEBUG2("PM:   Low power status --> ");
            if (m_lowPowerStatus == PM_LOW_POWER_MODE_ON)
            {
                PM_DEBUG("ON");
            } else {
                PM_DEBUG("OFF");
            }
#endif
            return true;
        }
    }
#endif
    return false;
}

