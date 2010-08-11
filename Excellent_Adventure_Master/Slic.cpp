#include "Slic.h"
#include <WProgram.h>



SLICControl :: SLICControl()
{
    m_offHookLocal = HOOK_ON;
    m_offHookRemote = HOOK_ON;
    m_debounceLocal = DEBOUNCE_UNDEFINED; // 0b10101010101010101010
    m_debounceRemote = DEBOUNCE_UNDEFINED; // 0b10101010101010101010
    m_time = millis();

    m_ringModeLocal = RINGING_OFF;
    m_ringModeRemote = RINGING_OFF;


}

void SLICControl :: InitSLICPins()
{
    pinMode(SLIC_PIN_SWITCH_HOOK_L, INPUT); 
    pinMode(SLIC_PIN_SWITCH_HOOK_R, INPUT);

    //pinMode(SLIC_PIN_FORWARD_REVERSE_L, OUTPUT);
    //pinMode(SLIC_PIN_FORWARD_REVERSE_R, OUTPUT);
    pinMode(SLIC_PIN_RING_MODE_L, OUTPUT);
    pinMode(SLIC_PIN_RING_MODE_R, OUTPUT);

    pinMode(SLIC_PIN_RINGER_L, OUTPUT);
    pinMode(SLIC_PIN_RINGER_R, OUTPUT);

    //SLIC 1 + 2 startup logic
    digitalWrite(SLIC_PIN_RING_MODE_L, RING_MODE_PIN_OFF);
    digitalWrite(SLIC_PIN_RING_MODE_R, RING_MODE_PIN_OFF);
    //digitalWrite(SLIC_PIN_FORWARD_REVERSE_L, HIGH);
    //digitalWrite(SLIC_PIN_FORWARD_REVERSE_R, HIGH);

    digitalWrite(SLIC_PIN_RINGER_L, RINGER_PIN_OFF);
    digitalWrite(SLIC_PIN_RINGER_R, RINGER_PIN_OFF);
}

// Non-blocking 16-sample debouncing polling.  Returns true if the signal is
// steady.
unsigned long SLICControl :: Poll()
{
    unsigned long time = millis();
    if ((time - m_time) > 2)
    {
        m_time = time;

        // Shift the bitfield down one, and append the last value read.
        m_debounceLocal = 
            (m_debounceLocal << 1) 
            |   (0x1 & digitalRead(SLIC_PIN_SWITCH_HOOK_L));
        m_debounceRemote = 
            (m_debounceRemote << 1) 
            |   (0x1 & digitalRead(SLIC_PIN_SWITCH_HOOK_R));
    }

    // If the values are steady, change the state.
    if 
    (   (   (m_debounceLocal == DEBOUNCE_ALL_ZEROS) 
        ||  (m_debounceLocal == DEBOUNCE_ALL_ONES)
        ) 
    &&  (   (m_debounceRemote == DEBOUNCE_ALL_ZEROS) 
        ||  (m_debounceRemote == DEBOUNCE_ALL_ONES)
        )
    )
    {
        m_offHookLocal  =   
            (m_debounceLocal == DEBOUNCE_ALL_ONES) 
            ?   HOOK_OFF 
            :   HOOK_ON;
        m_offHookRemote = 
            (m_debounceRemote == DEBOUNCE_ALL_ONES)
            ?   HOOK_OFF 
            :   HOOK_ON;

        //return true;
    }

    // Do the ringing:

    if (m_ringModeLocal == RINGING_ON)
    {
        RingLocal();
    }

    if (m_ringModeRemote == RINGING_ON)
    {
        RingRemote();
    }
//#ifdef DEBUG
//    Serial.print("DB: ");
//    Serial.print((unsigned)m_debounceLocal);
//    Serial.print(" ");
//    Serial.println((unsigned)m_debounceRemote);


    //return false;
    return m_time;
}

bool SLICControl :: IsOffHookLocal()
{
    return m_offHookLocal;
}

bool SLICControl :: IsOffHookRemote()
{
    return m_offHookRemote;
}

void SLICControl :: StartRingingLocal()
{
#ifdef SLIC_DEBUG
        Serial.println("LOCAL: START RINGING");
#endif
    m_ringModeLocal = RINGING_ON;
    m_isRingingLocal = RINGING_OFF;
    m_ringTimeDiffLocal = m_time & 0xFFFF;
    // The edge SHOULD be false
    m_ringEdgeLocal = true;
    m_ringCountLocal = 0;
    digitalWrite (SLIC_PIN_RING_MODE_L, RING_MODE_PIN_ON);
}

void SLICControl :: StartRingingRemote()
{
#ifdef SLIC_DEBUG
        Serial.println("REMOTE: START RINGING");
#endif

    m_ringModeRemote = RINGING_ON;
    m_isRingingRemote = RINGING_OFF;
    m_ringTimeDiffRemote = m_time & 0xFFFF;
    m_ringEdgeRemote = true;
    m_ringCountRemote = 0;
    digitalWrite (SLIC_PIN_RING_MODE_R, RING_MODE_PIN_ON);
}

void SLICControl :: StopRingingAll()
{
#ifdef SLIC_DEBUG
        Serial.println("ALL: STOP RINGING");
#endif
    StopRingingLocal();
    StopRingingRemote();
}


void SLICControl :: StopRingingLocal()
{
#ifdef SLIC_DEBUG
        Serial.println("LOCAL: STOP RINGING");
#endif
    m_ringModeLocal = RINGING_OFF;
    digitalWrite (SLIC_PIN_RING_MODE_L, RING_MODE_PIN_OFF);
    digitalWrite (SLIC_PIN_RINGER_L,    RINGER_PIN_OFF);
}

void SLICControl :: StopRingingRemote()
{
#ifdef SLIC_DEBUG
        Serial.println("REMOTE: STOP RINGING");
#endif

    m_ringModeRemote = RINGING_OFF;
    digitalWrite (SLIC_PIN_RING_MODE_R, RING_MODE_PIN_OFF);
    digitalWrite (SLIC_PIN_RINGER_R,    RINGER_PIN_OFF);
}
/*
void SLICControl :: RingLocal()
{
    // Clamp this to a range of [0,4096)
    unsigned int dTime = 0x0FFF & (m_time - (unsigned long)m_ringTimeDiffLocal);
    Serial.print("RL: ");
    Serial.println(dTime);
    if ((m_isRingingLocal == RINGING_OFF) && (dTime < RING_CADENCE))
    {
        digitalWrite(SLIC_PIN_RINGER_L, RINGER_PIN_ON);
        m_isRingingLocal = RINGING_ON;

#ifdef SLIC_DEBUG
        Serial.println("LOCAL: RING");
#endif

    } 
    else if ((m_isRingingLocal == RINGING_ON) && (dTime >= RING_CADENCE))
    {
        digitalWrite(SLIC_PIN_RINGER_L, RINGER_PIN_OFF);
        m_isRingingLocal = RINGING_OFF;
#ifdef SLIC_DEBUG
        Serial.println("LOCAL: NO RING");
#endif
    }

}
*/
void SLICControl :: RingLocal()
{
    // Every n milliseconds the state of second should switch from 0 to 1 
    bool second = (0x1 & ((m_time - (unsigned long)m_ringTimeDiffLocal) >> RING_CADENCE_UNIT));
    // If the value has changed:
    if (second != m_ringEdgeLocal)
    {
        m_ringEdgeLocal = second;
        // Increment the count
        m_ringCountLocal++; // Count from [0,3]
        if (m_ringCountLocal > (RING_CADENCE_DENOM-1))
        {
            // However, we want three-second counter, so if we get to 0,
            // increment to 1
            m_ringCountLocal = 0;
        }
    }

#ifdef SLIC_DEBUG
    Serial.print("RL: ");
    Serial.println(m_ringCountLocal);
#endif
    if ((m_isRingingLocal == RINGING_OFF) && (m_ringCountLocal < RING_CADENCE_NUMER))
    {
        digitalWrite(SLIC_PIN_RINGER_L, RINGER_PIN_ON);
        m_isRingingLocal = RINGING_ON;

#ifdef SLIC_DEBUG
        Serial.println("LOCAL: RING");
#endif

    } 
    else if ((m_isRingingLocal == RINGING_ON) && (m_ringCountLocal >= RING_CADENCE_NUMER))
    {
        digitalWrite(SLIC_PIN_RINGER_L, RINGER_PIN_OFF);
        m_isRingingLocal = RINGING_OFF;
#ifdef SLIC_DEBUG
        Serial.println("LOCAL: NO RING");
#endif
    }

}


void SLICControl :: RingRemote()
{
    // Every n milliseconds the state of second should switch from 0 to 1 
    bool second = (0x1 & ((m_time - (unsigned long)m_ringTimeDiffRemote) >> RING_CADENCE_UNIT));
    // If the value has changed:
    if (second != m_ringEdgeRemote)
    {
        m_ringEdgeRemote = second;
        // Increment the count
        m_ringCountRemote++; // Count from [0,3]
        if (m_ringCountRemote > (RING_CADENCE_DENOM-1))
        {
            // However, we want three-second counter, so if we get to 0,
            // increment to 1
            m_ringCountRemote = 0;
        }
    }

#ifdef SLIC_DEBUG
    Serial.print("RR: ");
    Serial.println(m_ringCountRemote);
#endif
    // One second on, two seconds off.
    if ((m_isRingingRemote == RINGING_OFF) && (m_ringCountRemote < RING_CADENCE_NUMER))
    {
        digitalWrite(SLIC_PIN_RINGER_R, RINGER_PIN_ON);
        m_isRingingRemote = RINGING_ON;
#ifdef SLIC_DEBUG
        Serial.println("REMOTE: RING");
#endif
    } 
    else if ((m_isRingingRemote == RINGING_ON) && (m_ringCountRemote >= RING_CADENCE_NUMER))
    {
        digitalWrite(SLIC_PIN_RINGER_R, RINGER_PIN_OFF);
        m_isRingingRemote = RINGING_OFF;
#ifdef SLIC_DEBUG
        Serial.println("REMOTE: NO RING");
#endif
    }

}
