/*
Excellent Adventure Test #1
LED fader for prototype of Excellent Adventure light pods

Gustavo Huber - gush@carbonworkshop.com
Carbon Workshop
April 8 2010
 */


#include "WProgram.h"
void setup();
void loop();
int red = 3;    // Base of Q for red LEDs connected w/ 1K resistor to Arduino pin 3
int grn = 5;    // Base of Q for green LEDs connected w/ 1K resistor to Arduino pin 5
int blu = 6;    // Base of Q for blue LEDs connected w/ 1K resistor to Arduino pin 6
int wht = 9;    // Base of Q for white LEDs connected w/ 1K resistor to Arduino pin 9
int ww = 10;    // Base of Q for warm white LEDs connected w/ 1K resistor to Arduino pin 10
int uv = 11;    // Base of Q for UV LEDs connected w/ 1K resistor to Arduino pin 11

void setup()  { 
  // nothing happens in setup 
} 

void loop()  { 
  // FADE IN RED
  // fade RED LEDs in from min to max in increments of 5 points:
  for(int redValue = 0 ; redValue <= 255; redValue +=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(red, redValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 

  // fade RED LEDs out from max to min in increments of 5 points:
  for(int redValue = 255 ; redValue >= 0; redValue -=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(red, redValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 
  // END RED
  
  delay(500);
  
  // FADE IN GREEN
  // fade GREEN LEDs in from min to max in increments of 5 points:
  for(int grnValue = 0 ; grnValue <= 255; grnValue +=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(grn, grnValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 

  // fade GREEN LEDs out from max to min in increments of 5 points:
  for(int grnValue = 255 ; grnValue >= 0; grnValue -=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(grn, grnValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 
  // END GREEN
  
  delay(500);
  
  // FADE IN BLUE
  // fade BLUE LEDs in from min to max in increments of 5 points:
  for(int bluValue = 0 ; bluValue <= 255; bluValue +=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(blu, bluValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 

  // fade BLUE LEDs out from max to min in increments of 5 points:
  for(int bluValue = 255 ; bluValue >= 0; bluValue -=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(blu, bluValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 
  // END BLUE
  
  delay(500);
  
  // FADE IN WHITE
  // fade WHITE LEDs in from min to max in increments of 5 points:
  for(int whtValue = 0 ; whtValue <= 255; whtValue +=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(wht, whtValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 

  // fade WHITE LEDs out from max to min in increments of 5 points:
  for(int whtValue = 255 ; whtValue >= 0; whtValue -=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(wht, whtValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 
  // END WHITE
  
  delay(500);
  
  // FADE IN WARM WHITE
  // fade WARM WHITE LEDs in from min to max in increments of 5 points:
  for(int wwValue = 0 ; wwValue <= 255; wwValue +=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(ww, wwValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 

  // fade WARM WHITE LEDs out from max to min in increments of 5 points:
  for(int wwValue = 255 ; wwValue >= 0; wwValue -=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(ww, wwValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 
  // END WARM WHITE
  
  delay(500);
  
  // FADE IN UV
  // fade UV LEDs in from min to max in increments of 5 points:
  for(int uvValue = 0 ; uvValue <= 255; uvValue +=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(uv, uvValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 

  // fade UV LEDs out from max to min in increments of 5 points:
  for(int uvValue = 255 ; uvValue >= 0; uvValue -=5) { 
    // sets the value (range from 0 to 255):
    analogWrite(uv, uvValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);                            
  } 
  // END UV
  
  delay(2500);
}



int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

