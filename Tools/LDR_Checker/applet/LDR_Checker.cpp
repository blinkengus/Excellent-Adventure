/*
  Analog input, serial output
 
 Reads an analog input pin, prints the results to the serial monitor.
 
 The circuit:

 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 
 created over and over again
 by Tom Igoe and everyone who's ever used Arduino
 
 */
 
 #include "WProgram.h"
void setup();
void loop();
int threshold = 512;
 
 void setup() {
  Serial.begin(38400); 
  digitalWrite(13, LOW);
 }
 
 void loop() {
  // read the analog input into a variable:
   int analogValue = analogRead(1);
   // print the result:
   Serial.println(analogValue);
   // wait 10 milliseconds for the analog-to-digital converter
   // to settle after the last reading:
  // while(analogValue < threshold){
    // digitalWrite(13, HIGH);}
    // digitalWrite(13, LOW);
   delay(10);
 }

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

