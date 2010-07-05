/*
 * Blinky Panel Scanner -- 
 *
 *
 * Blinky Panel connections to Arduino
 * PWR - -- gnd -- black -- Gnd
 * PWR + -- +5V -- red   -- 5V
 * I2C d -- SDA -- green -- Analog In 4
 * I2C c -- SCK -- blue  -- Analog In 5
 *
 *
 * 2007, Tod E. Kurt, ThingM, http://thingm.com/
 *
 */


#include "Wire.h"
#include "BlinkM_funcs.h"
int fadespeed = 3;
int fadedelay = 2000;
int strobedelay = 15;
int strobecount = 25;
int randomcolorcount = 250;

void setup()
{

    BlinkM_beginWithPower();

    Serial.begin(19200);
    BlinkM_stopScript(0);
    BlinkM_setFadeSpeed(0, 50);
    BlinkM_setRGB(0, 0x00, 0x00, 0x00);
    delay(1000);
}


void loop() {
  randomcolors();
  whitefadeout();
  redfadeout();
  greenfadeout();
  bluefadeout();
  whitestrobe();
  
  
  
}

void randomcolors(){
  for(int i = 0; i < randomcolorcount; i++){
 
  int r = random(255);
  int g = random(255);
  int b = random(255);
  
  //BlinkM_fadeToRGB( 0, 0x00, 0x00, 0x00);
  
  //for(int p = 21; p < 23; p ++){
    int p = random(0, 25);
    Serial.println(p);
    //BlinkM_setRGB( p, 0x00, 0x00, 0x00 );
    //delay(50);
    BlinkM_setFadeSpeed( p, random(25));
    BlinkM_fadeToRGB( p, (byte)r, (byte)g, (byte)b  );
    delay(random(100));
    //BlinkM_setFadeSpeed( p, random(100));
    //BlinkM_fadeToRGB( p, 0x00, 0x00, 0x00);
    //delay (random(250));
  //}


  //delay(750);
}
}

void whitefadeout(){
  BlinkM_setFadeSpeed(0, fadespeed);
  BlinkM_fadeToRGB(0, 255, 255, 255);
  delay(fadedelay);
  BlinkM_fadeToRGB(0, 0, 0, 0);
  delay(fadedelay);
}

void redfadeout(){
  BlinkM_setFadeSpeed(0, fadespeed);
  BlinkM_fadeToRGB(0, 255, 0, 0);
  delay(fadedelay);
  BlinkM_fadeToRGB(0, 0, 0, 0);
  delay(fadedelay);
}

void greenfadeout(){
  BlinkM_setFadeSpeed(0, fadespeed);
  BlinkM_fadeToRGB(0, 0, 255, 0);
  delay(fadedelay);
  BlinkM_fadeToRGB(0, 0, 0, 0);
  delay(fadedelay);
}

void bluefadeout(){
  BlinkM_setFadeSpeed(0, fadespeed);
  BlinkM_fadeToRGB(0, 0, 0, 255);
  delay(fadedelay);
  BlinkM_fadeToRGB(0, 0, 0, 0);
  delay(fadedelay);
}

void whitestrobe(){
//  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
  BlinkM_setRGB(0, 255, 255, 255);
  delay(strobedelay);
  BlinkM_setRGB(0, 0, 0, 0);
  delay(strobedelay);
  }
}
