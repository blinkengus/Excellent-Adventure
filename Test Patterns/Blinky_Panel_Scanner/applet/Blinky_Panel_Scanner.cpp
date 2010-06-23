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

#include "WProgram.h"
void setup();
void loop();
void setup()
{

    BlinkM_beginWithPower();

    Serial.begin(19200);
    BlinkM_stopScript(0);
    BlinkM_setFadeSpeed(0, 50);
    BlinkM_setRGB(0, 0x00, 0x00, 0x00);
}


void loop() {
  
  int r = random(255);
  int g = random(255);
  int b = random(255);
  
  //BlinkM_fadeToRGB( 0, 0x00, 0x00, 0x00);
  
  for(int p = 1; p < 21; p ++){
    //int p = random(1, 21);
    Serial.println(p);
    //BlinkM_setRGB( p, 0x00, 0x00, 0x00 );
    //delay(50);
    BlinkM_setFadeSpeed( p, r);
    BlinkM_fadeToRGB( p, (byte)r, (byte)g, (byte)b  );
    delay(g);
    BlinkM_setFadeSpeed( p, 5);
    BlinkM_fadeToRGB( p, 0x00, 0x00, 0x00);
    delay (random(255));
  }


  //delay(750);
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

