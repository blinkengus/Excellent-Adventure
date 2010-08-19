#include "Spectrum.h"
#include <WProgram.h>
#include "Globals.h"

#ifndef SPECTRUM_USE_HARDWARE
// Fixed point bands:
static int bandConsts[SPECTRUM_BANDS] =
{ 
    32717   // 85
,   32488   // 200
,   32138   // 300
,   31029   // 500
,   28378   // 800
,   25997   // 1000
,   18205   // 1500
//,   8481    // 2000
};

#endif


Spectrum :: Spectrum()
{
    for (char i = 0; i < SPECTRUM_BANDS; i++)
    {
        m_spectrum[i] = 0;
    }
}

Spectrum :: ~Spectrum()
{
}

void Spectrum :: Destroy()
{
}

void Spectrum :: InitSpectrumPins()
{
    pinMode(SPECTRUM_PIN_ANALOG, INPUT); 
    pinMode(SPECTRUM_PIN_STROBE, OUTPUT); 
    pinMode(SPECTRUM_PIN_RESET, OUTPUT);
#ifdef USE_ARDUINO
    analogReference(DEFAULT);
#endif
    digitalWrite(SPECTRUM_PIN_RESET, LOW);
    digitalWrite(SPECTRUM_PIN_STROBE, HIGH);   
}

#ifndef SPECTRUM_USE_HARDWARE
#define FIXED_Q           15
#define FIXED_K           (1 << (FIXED_Q-1))


int fmult(long int a, long int b)
{
    long int r = a * b;
    r += FIXED_K;
    return (r >> FIXED_Q);
}

int power(int c, int p, int p2)
{
    return fmult(p,p) + fmult(p2,p2) - fmult(p,fmult(p2,c));
}
#endif


unsigned short * Spectrum :: ReadSpectrum ()
{
#ifdef SPECTRUM_USE_HARDWARE
    // If we're connected to a MSGEQ7 spectrum analysis chip
    digitalWrite(SPECTRUM_PIN_RESET, HIGH);
    digitalWrite(SPECTRUM_PIN_RESET, LOW);

    for (char i = 0; i < SPECTRUM_BANDS; i++)
    {
        digitalWrite(SPECTRUM_PIN_STROBE, LOW);
        // Settling time
        delayMicroseconds(30);
        m_spectrum[i] = analogRead(SPECTRUM_PIN_ANALOG);
        digitalWrite(SPECTRUM_PIN_STROBE, HIGH);
    }
#else
// Implement Goertzel algorithm for a crude spectral analysis for specific
// frequencies (which we call "bands").

// omega = [frequencies of interest]
// for (x(n) = samples(n))
// {
//      for (i as frequencies of interest)
//      {
//          C_i = 2cos(2pi*omega_i)
//          s_i(0) = x_i(n) + C_i*s_i(1) - s_i(2)
//      }
// }
// power_i = s_i(0)^2 + s_i(1)^2 - s_i(0)*s_i(1)

// ADC prescale = 128
// Atmega chip 16Mhz / prescale = 125khz
// ADC conversion ~ 13 clocks
// f_Nq ~ 125khz / 13 ~ 9.6khz

// omega = 2pi k / N
// N = block size
// k = N * f / f_Nq

// We will use N = 64

// 85, 200, 300, 500, 800, 1000, 1500, 2000, 3000, 5000 (4200)
//     ^    ^    ^    ^    ^     ^     ^     ^
//     ^         ^         ^           ^           ^

// 10-bit ADC, i.e. +/- 512
/*
   Fixed Q15 bands:

   function () 
   {
       var m = 1;
       var n = 15;
       var Q1_15 = pow(2, n);
       var bands = [200, 500, 1000, 2000, 4200];
       var f_s = 9600;
       var out = [];
       for (var i = 0; i < bands.length; i++) 
       {
           var f = bands[i];
           var w = 2 * pi * f / f_s;
           var C = 2 * cos(w);
           out.push(Math.round(pow(2, n) * (C / 2)));
       }
       return out;
   }
 */
    int band[SPECTRUM_BANDS][2];
    char b = SPECTRUM_BANDS-1;
    do
    {
        band[b][0] = 0;
        band[b][1] = 0;
    } while (b--);

    char sample = SPECTRUM_BLOCKSIZE;
    do
    {
        b = SPECTRUM_BANDS-1;
        // Read in the value, center it and chop off the noise:
        int x = ((analogRead(SPECTRUM_PIN_ANALOG) - 512) >> 1 << 7) 
                >> SPECTRUM_REDUCTION_FACTOR;
        do
        {
            int p = band[b][0];
            int p2 = band[b][1];
            int s = x + fmult(bandConsts[b] << 1, p) - p2;
            band[b][1] = band[b][0];
            band[b][0] = s;

        } while(b--);
    } while (sample--);

    // Now compute the power and scale it as needed:
    b = SPECTRUM_BANDS-1;
    do
    {
        m_spectrum[b] = power(bandConsts[b], band[b][0], band[b][1]) >> 7;
    } while (b--);

#endif
    return &m_spectrum[0];
}
unsigned short * Spectrum :: GetSpectrum()
{
    return &m_spectrum[0];
}