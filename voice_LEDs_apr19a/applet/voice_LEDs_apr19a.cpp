
#include "WProgram.h"
void setup();
void loop();
int pin_mic = 2;              // mic input on analog pin 2
int led1 = 9;                 // LED 1 on digital pin 9
int led2 = 10;                // LED 2 on digital pin 10
int led3 = 11;                // LED 3 on digital pin 11
int val1 = 0;
int val2 = 0;
int val3 = 0;
int mic_val = 0;              // initial mic value is set to 0
  
void setup()                    
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  Serial.begin(9600);
  Serial.println("ready");

}

void loop() {  
  mic_val = analogRead(pin_mic);
  val1 = mic_val - 700;
  val2 = mic_val - 600;
  val3 = mic_val - 500;
  analogWrite(led1, val1);
  analogWrite(led2, val2);
  analogWrite(led3, val3);
  // print the value
  Serial.print("mic_val = ");
  Serial.print(mic_val);
  Serial.print(" / ");
  Serial.print(val1);
  Serial.print(" / ");
  Serial.println(val3);
  delay(100);
}






int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

