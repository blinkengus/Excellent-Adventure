#define BANDS     8
#define BLOCKSIZE 64
                 
#define Q         15
                 
#define K         (1 << (Q-1))

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

#define REDUCTION_FACTOR 3

int blocks = BLOCKSIZE - 1;
int x0;
int sync;
#define FRAMESIZE BANDS
byte syncFrame[BANDS];

unsigned long time0;
unsigned long totalTime = 0;

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

void processFFT()
{
    // f_s ~ 20ms

    //return;
    int b = BANDS-1;
    if(blocks <= 0){
        //if (sync <= 0)
        //{
        //    Serial.write(syncFrame, FRAMESIZE);
        //    sync = 5;
        //}

        byte frame[BANDS];
        for(int i = 0; i < BANDS; i++){
            int p = power(consts[i], band[i][0], band[i][1]) >> 7;
            frame[i] = p;
            //Serial.print(p);
            //Serial.print("\t");
        }
        
        Serial.write(frame, FRAMESIZE);

        do{
            band[b][0] = 0;
            band[b][1] = 0;
        }
        while(b--);
        
        blocks = BLOCKSIZE - 1;
        //Serial.print("\n");
        //Serial.println(totalTime);
        //totalTime = 0;
        b = BANDS - 1;
        //sync--;
    }

    int xReal = (micmult * (analogRead(micpin) - micbias) >> 1 << 7) >> REDUCTION_FACTOR;
    int x = xReal;
    do{
        int p = band[b][0];
        int p2 = band[b][1];
        int s = x + fmult(consts[b] << 1,p) - p2;
        band[b][1] = band[b][0];
        band[b][0] = s;
    }
    while(b--);
    
    blocks--;
    x0 = xReal;
}