// RECEIVER

//byte incomingByte, SensorVal;
#include "WProgram.h"
void setup();
void loop();
byte SerialRead;

void setup() {

// start serial port at 19200 bps
Serial.begin(9600);
//Serial.println("Ready! ");

// led pins

pinMode (10, OUTPUT);
//pinMode (6, OUTPUT);

//delay(150);

}

void loop() {
//if (Serial.available()) {   // are there any bytes available on the serial port ???

// assign bytes to the var \u2018incomingByte\u2019
SerialRead = Serial.read();

Serial.println(SerialRead);

// from now on is pretty clear I guess   :)

if(SerialRead = 1){
digitalWrite(10, HIGH);
}
else
{
digitalWrite (10, LOW);
}
//}

}



int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

