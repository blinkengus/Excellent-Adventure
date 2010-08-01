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
#include "BlinkM_funcs.h"

#define PERIOD_MICROSEC 100000
#define PANELS_WIDTH    9
#define PANELS_HEIGHT   9

#define MAX_COLOR       240

EffectManager EM(PERIOD_MICROSEC, PANELS_WIDTH, PANELS_HEIGHT);


int FxWave(Canvas *canvas, char mode)
{
    static int yo = 0;

    char w = canvas->GetWidth();
    char h = canvas->GetHeight();
    for(char y = 0; y < h; y++){
        for(char x = 0; x < w; x++){
            canvas->PutPixel(x, y, constrain(sin((y * 4 + yo) * 0.5f) * 120 + 120, 0, 240), 0, 0);
        }
    }

    yo++;
}

int FxCount(Canvas *canvas, char mode)
{
    static int xi, yi;

    if(mode == EFFECTMODE_INTRO){
        xi = 0; yi = 0;
    }

    char w = canvas->GetWidth();
    char h = canvas->GetHeight();
    for(char y = 0; y < h; y++){
        for(char x = 0; x < w; x++){
            if(xi == x && yi == y)
                canvas->PutPixel(x, y, 0xf0, 0x00, 0x00);
            else
                canvas->PutPixel(x, y, 0x00, 0x00, 0x00);
        }
    }

    xi++;
    if(xi == PANELS_WIDTH){
        xi = 0;
        yi++;
        if(yi == PANELS_HEIGHT)
            yi = 0;
    }
}

int FxGrad(Canvas *canvas, char mode)
{
    for(char y = 0; y < PANELS_HEIGHT; y++){
        for(char x = 0; x < PANELS_WIDTH; x++){
            canvas->PutPixel(x, y, x * 24, 0, y * 24);
        }
    }
}

float clamp(float v, float vmin, float vmax)
{
    if(v < vmin)
        return vmin;
    if(v > vmax)
        return vmax;
    return v;
}

int FxRipple(Canvas *canvas, char mode)
{
    static char current_mode = -1;
    static float vel_r[PANELS_HEIGHT][PANELS_WIDTH];
    static float vel_g[PANELS_HEIGHT][PANELS_WIDTH];
    static float vel_b[PANELS_HEIGHT][PANELS_WIDTH];
    static int loop_count = 0;

    static float damping = 0.9f;
    static float spread  = 0.1f;

    char w1 = PANELS_WIDTH - 1;
    char h1 = PANELS_HEIGHT - 1;

    if(loop_count == 0){
        for(char y = 0; y < PANELS_HEIGHT; y++)
            for(char x = 0; x < PANELS_WIDTH; x++)
                vel_r[y][x] = 0.0f;//0.0f;
    }

    // if(mode != current_mode){
    //     switch(mode){
    //         case EFFECTMODE_INTRO:
    //             for(char y = 0; y < h; y++)
    //                 for(char x = 0; x < w; x++)
    //                     vel_r[y][x] = 0.0f;
    //             loop_count = 0;
    //             break;
    //         case EFFECTMODE_LOOP:  break;
    //         case EFFECTMODE_OUTRO: break;
    //     }
    //     current_mode = mode;
    // }

    if(loop_count % 20 == 0){
        vel_r[random(PANELS_HEIGHT)][random(PANELS_WIDTH)] = 1000;
    }
    if((loop_count + 20) % 20 == 0){
        vel_g[random(PANELS_HEIGHT)][random(PANELS_WIDTH)] = 1000;
    }
    if((loop_count + 40) % 20 == 0){
        vel_b[random(PANELS_HEIGHT)][random(PANELS_WIDTH)] = 1000;
    }

    float posr, posg, posb, vr, vg, vb, offr, offg, offb;
    for(char y = 0; y < PANELS_HEIGHT; y++){
        for(char x = 0; x < PANELS_WIDTH; x++){
            posr = float(canvas->GetR(x, y));
            posg = float(canvas->GetG(x, y));
            posb = float(canvas->GetB(x, y));
            vr = vel_r[y][x] * damping;
            vg = vel_g[y][x] * damping;
            vb = vel_b[y][x] * damping;

            if(x > 0){
                offr = float(canvas->GetR(x - 1, y)) - posr;
                offg = float(canvas->GetG(x - 1, y)) - posg;
                offb = float(canvas->GetB(x - 1, y)) - posb;
                vr += offr * spread;
                vg += offg * spread;
                vb += offb * spread;
            }
            if(y > 0){
                offr = float(canvas->GetR(x, y - 1)) - posr;
                offg = float(canvas->GetG(x, y - 1)) - posg;
                offb = float(canvas->GetB(x, y - 1)) - posb;
                vr += offr * spread;
                vg += offg * spread;
                vb += offb * spread;
            }
            if(x < w1){
                offr = float(canvas->GetR(x + 1, y)) - posr;
                offg = float(canvas->GetG(x + 1, y)) - posg;
                offb = float(canvas->GetB(x + 1, y)) - posb;
                vr += offr * spread;
                vg += offg * spread;
                vb += offb * spread;
            }
            if(y < h1){
                offr = float(canvas->GetR(x, y + 1)) - posr;
                offg = float(canvas->GetG(x, y + 1)) - posg;
                offb = float(canvas->GetB(x, y + 1)) - posb;
                vr += offr * spread;
                vg += offg * spread;
                vb += offb * spread;
            }

            vel_r[y][x] = vr;
            vel_g[y][x] = vg;
            vel_b[y][x] = vb;
        }
    }

    //Serial.println((uint32_t)canvas->GetB(0, 0));

    for(char y = 0; y < PANELS_HEIGHT; y++){
        for(char x = 0; x < PANELS_WIDTH; x++){
            canvas->PutPixel(x, y,
                clamp(float(canvas->GetR(x, y)) - 5.0f + vel_r[y][x], 0.0f, MAX_COLOR),
                clamp(float(canvas->GetG(x, y)) - 5.0f + vel_g[y][x], 0.0f, MAX_COLOR),
                clamp(float(canvas->GetB(x, y)) - 5.0f + vel_b[y][x], 0.0f, MAX_COLOR)
            );
        }
    }

    loop_count++;
}


Effect effects[1] = {
    { &FxRipple, 0 }
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
    EM.Callback();
    //delay(5);
}