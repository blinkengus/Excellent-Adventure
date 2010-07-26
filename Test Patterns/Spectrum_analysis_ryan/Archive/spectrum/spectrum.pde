
#define FASTADC 0

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
const int micpin = 2;
const int micmultiplier = 10;
const int micbiaspoint = 512;




/*
// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// set prescale to 16
sbi(ADCSRA,ADPS2) ;
cbi(ADCSRA,ADPS1) ;
cbi(ADCSRA,ADPS0) ;
// REF:  http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
 */



// s(n) = x(n) + C*s(n-1) - s(n-2)
// C = 2cos(2pi*omega)

// [freq][C, s(n-1), s(n-2)]

// ADC prescale = 128
// Atmega chip 16Mhz / prescale = 125khz
// ADC conversion ~ 13 clocks
// f_Nq ~ 125khz / 13 ~ 9.6khz

// omega = 2pi k / N
// N = block size
// k = N * f / f_Nq

// We will use N = 200

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
#define BANDS       8
#define BLOCKSIZE   128

#define Q           15

#define K           (1 << (Q-1))

int band[BANDS][2];

// Fixed point bands:
int consts[BANDS] =
{ 
    32717   // 85
,   32488   // 200
,   32138   // 300
,   31029   // 500
,   28378   // 800
,   25997   // 1000
,   18205   // 1500
,   8481    // 2000
};

int blocks = BLOCKSIZE-1;
int sync;
#define FRAMESIZE BANDS
byte syncFrame[BANDS];

unsigned long time0;
unsigned long totalTime = 0;

void setup() 
{
#if FASTADC
    // set prescale to 16
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
#endif

    int b = BANDS-1;

    do
    {
        band[b][0] = 0;
        band[b][1] = 0;
    } while (b--);

    Serial.begin(9600);
    //Serial.println("Begin:");
}


int fmult(long int a, long int b)
{
    long int r = a * b;
    r += K;
    return (r >> Q);
}

int power(int c, int p, int p2)
{
    return fmult(p,p) + fmult(p2,p2) - fmult(p,fmult(p2,c));
}

void loop() 
{
    // f_s ~ 20ms

    //return;
    int b = BANDS-1;
    if (blocks <= 0)
    {
        //if (sync <= 0)
        //{
        //    Serial.write(syncFrame, FRAMESIZE);
        //    sync = 5;
        //}

        byte frame[BANDS];
        for (int i = 0; i < BANDS; i++)
        {
            int p = power(consts[i], band[i][0], band[i][1]) >> 8;
            frame[i] = p;
            //Serial.print(p);
            //Serial.print("\t");
        }
        
        Serial.write(frame, FRAMESIZE);

        do
        {
            band[b][0] = 0;
            band[b][1] = 0;
        } while (b--);
        blocks = BLOCKSIZE-1;
        //Serial.print("\n");
        //Serial.println(totalTime);
        //totalTime = 0;
        b = BANDS-1;
        //sync--;
    }
    //Serial.println(x);
    //return;
    //int x = (analogRead(0) - 512) << 6;
    //int x; 
    //x += random(-1,1);//blocks - (BLOCKSIZE / 2);

    //time0 = millis();
    int x = (micmultiplier * (analogRead(micpin) - micbiaspoint)) << 4;
    do
    {
        int p = band[b][0];
        int p2 = band[b][1];
        long int c = (consts[b] << 1); // The constant value is actually  * 2
        int cp = fmult(c,p); // fixed mult
        int s = x + cp - p2;
        band[b][1] = band[b][0];
        band[b][0] = s;

    } while(b--);
    blocks--;
   
    //totalTime += (millis() - time0);
}

