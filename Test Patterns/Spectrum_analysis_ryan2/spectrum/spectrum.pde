
#define FASTADC 0

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

const int micpin = 2;
const int micmult = 1;
const int micbias = 512;

void setup()
{
#if FASTADC
    // set prescale to 16
    sbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    cbi(ADCSRA, ADPS0);
#endif

    int b = BANDS - 1;
    x0 = 0;
    do{
        band[b][0] = 0;
        band[b][1] = 0;
    }
    while(b--);

    Serial.begin(9600);
}


void loop() 
{
    processFFT();
}

