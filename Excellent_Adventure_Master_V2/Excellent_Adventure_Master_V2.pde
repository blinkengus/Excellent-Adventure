/*
   ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
   /\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
   \ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
    \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
     \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/

   ---What?---
   Combined "master" firmware for Excellent Adventure.
   Controls LED panels, dumb LEDs, SLIC, and takes analog
   sound and ambient light inputs.

   ---Why?---
   www.carbonworkshop.com/bm10

   ---Who?---
   Chris Condrat - chris@g6net.com
   Gustavo Huber - gush@carbonworkshop.com
   Daniel Massey - pichiste@gmail.com

   ---When?---
   June 11, 2010
 */

#include <Wire.h>
#include <WProgram.h>
#include <TimerOne.h>
#include "EffectManager.h"

#define PERIOD_MICROSEC 100000
#define PANELS_WIDTH    9
#define PANELS_HEIGHT   10

EffectManager EM(PERIOD_MICROSEC, PANELS_WIDTH, PANELS_HEIGHT);

int SimpleColumns(Canvas *c, char mode)
{
    static int n = 0;
    static char currentMode = -1;
    if (mode != currentMode)
    {
        // Performed only once.
        switch(mode){
            case EFFECTMODE_INTRO:
                n = 0;
                break;
            case EFFECTMODE_LOOP:
                break;
            case EFFECTMODE_OUTRO:
                break;
        }
        currentMode = mode;
    } else {
        n = (n+1) & 0x7;
    }


    char h = c->GetHeight();
    for (char y = 0; y < h; y++)
    {
        // Create ascending values
        // [0, 
        
        int nn = (((n+y) << 5)-1) & 0xFF;
        /*
        uint32_t color = COLOR(0,nn,0);
        c->PutPixel(0, y, color);
        c->PutPixel(2, y, color);
        c->PutPixel(3, y, color);
        c->PutPixel(5, y, color);
        c->PutPixel(6, y, color);
        c->PutPixel(8, y, color);
        */
        
        c->PutPixel(0, y, COLOR(nn,0,0));
        c->PutPixel(2, y, COLOR(0,nn,0));

        c->PutPixel(3, y, COLOR(0,0,nn));
        c->PutPixel(5, y, COLOR(nn,nn,0));

        c->PutPixel(6, y, COLOR(nn,0,nn));
        c->PutPixel(8, y, COLOR(0,nn,nn));
    }
}


int FxWave(Canvas *canvas, char mode)
{
    static int yo = 0;
    
    char w = canvas->GetWidth();
    char h = canvas->GetHeight();
    for(char y = 0; y < h; y++){
        for(char x = 0; x < w; x++){
            canvas->PutPixel(x, y, COLOR(int(sin((y + yo) * 0.5f) * 128 + 128), 0, 0));
        }
    }
    
    yo++;
}


Effect effects[2] = {
    { &FxWave, 0 }
};

void setup()
{
    Serial.begin(38400);
    EM.AddEffectsArrays(
        effects, 1, 
        effects, 1, 
        effects, 1
    );
    EM.SetMode(EM_MODE_IDLE);
    EM.InstallAnimator();
}


void loop()
{
}