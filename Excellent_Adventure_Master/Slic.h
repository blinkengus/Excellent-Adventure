#ifndef _SLIC_H
#define _SLIC_H

// Ring Mode
// Bias conditions during ringing. Set to 1 during ringing, 0 other.
#define SLIC_PIN_RING_MODE_L        4
#define SLIC_PIN_RING_MODE_R        7


// Forward/Reverse
// Logic 0 reverses Tip and Ring.  Toggled to produce ring output.

// NOT USED:
//#define SLIC_PIN_FORWARD_REVERSE_L  3
//#define SLIC_PIN_FORWARD_REVERSE_R  8

// Switch-hook
// Indicates off-hook condition when 1.
#define SLIC_PIN_SWITCH_HOOK_L      2
#define SLIC_PIN_SWITCH_HOOK_R      12

// CHANGE THESE:

#define SLIC_PIN_RINGER_L           3
#define SLIC_PIN_RINGER_R           8


#define DEBOUNCE_ALL_ONES           0xFFFF
#define DEBOUNCE_ALL_ZEROS          0x0000
#define DEBOUNCE_UNDEFINED          0xAAAA


#define RINGING_ON                  true
#define RINGING_OFF                 false

// Active LOW
#define RINGER_PIN_ON               LOW
#define RINGER_PIN_OFF              HIGH

#define RING_MODE_PIN_ON            HIGH
#define RING_MODE_PIN_OFF           LOW

#define HOOK_PIN_ON                 LOW
#define HOOK_PIN_OFF                HIGH


#define HOOK_ON                     false
#define HOOK_OFF                    true

// Ring cadence is the fraction of 4096 that the ring is on.

// 9 = 512 milliseconds
// 10 = 1024 milliseconds
// etc.
#define RING_CADENCE_UNIT           9

// The ring is a fraction 
#define RING_CADENCE_NUMER          3
#define RING_CADENCE_DENOM          6

//#define SLIC_DEBUG




class SLICControl
{

    bool                                            m_offHookLocal;
    bool                                            m_offHookRemote;
    int                                             m_debounceLocal;
    int                                             m_debounceRemote;

    unsigned long                                   m_time;
    bool                                            m_ringModeLocal;
    bool                                            m_ringModeRemote;

    bool                                            m_isRingingLocal;
    bool                                            m_isRingingRemote;
    // "Edge detection" for full second counts
    bool                                            m_ringEdgeLocal;
    bool                                            m_ringEdgeRemote;
    char                                            m_ringCountLocal;
    char                                            m_ringCountRemote;
    // So that we start ringing on a full ring, rather than a midway ring, we
    // store the time difference from the clock.

    unsigned int                                    m_ringTimeDiffLocal;
    unsigned int                                    m_ringTimeDiffRemote;

public:
    SLICControl();

    void InitSLICPins();

    // Non-blocking poll.  Also handles the ringing when the ringing is
    // started.  Returns the milliseconds timestamp.
    unsigned long Poll();

    bool IsOffHookLocal();
    bool IsOffHookRemote();

    // Start the ringing when you want it to ring for that phone.

    void StartRingingLocal();
    void StartRingingRemote();

    void StopRingingAll();
    void StopRingingLocal();
    void StopRingingRemote();

protected:

    void RingLocal();
    void RingRemote();

};






#endif
