#ifndef _SPECTRUM_H
#define _SPECTRUM_H

#include "Globals.h"

#define SPECTRUM_BANDS                  7


// How many samples to analyze over
#define SPECTRUM_BLOCKSIZE              64

class Spectrum
{
    unsigned short                                  m_spectrum[SPECTRUM_BANDS];
public:
    Spectrum();
    ~Spectrum();

    void InitSpectrumPins();

    void Destroy();
    unsigned short *ReadSpectrum();
    unsigned short *GetSpectrum();
};


#endif