#include "WProgram.h"
void setup();
void loop();
void ring();
const int ringPin = 2;
const int enablePin = 3;
const int buttonPin = 4;
const int ringDelay = 50; //50ms = 20Hz
const int ringSpacing = 250;

void setup() {
  Serial.begin(9600);
  Serial.println("Listo!");
  pinMode(ringPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ringPin, HIGH);
  digitalWrite(enablePin, LOW);
  Serial.println("RM LOW LOW LOW LOW LOW");
}

void loop() {
  while (digitalRead(buttonPin) != HIGH){ }
    digitalWrite(enablePin, HIGH);
    Serial.println("Ringer on!");
    delay(10);
//    for( int i = 0; i < 5; i++) {
    ring();
//    Serial.println(i);
    delay(ringSpacing);
//    }
//  }
 
  digitalWrite(enablePin, LOW);
  Serial.println("Ringer off!");
  Serial.print(analogRead(2));
  Serial.print("  /  ");
  Serial.println(analogRead(3));
  
}

void ring() {
  for(int i = 0; i < 5; i++) {
    digitalWrite(ringPin, LOW);
    Serial.print("di");
    delay(ringDelay);
    digitalWrite(ringPin, HIGH);
    Serial.print("ng ");
    delay(ringDelay);
    
  }
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

