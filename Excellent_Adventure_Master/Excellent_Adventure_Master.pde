#include "Wire.h" 
#include "BlinkM_funcs.h" 

const int NUM_BLINKMS = 4;
const byte FIRST_BLINKM_ADDR = 1;

const int ringPin = 2;
const int enablePin = 3;
const int buttonPin = 4;
const int ringDelay = 40; //50ms = 20Hz
const int ringSpacing = 220;
const int noiseLevel = 505;
const int threshold = 10;
const int idleBright = 0;

void setup() {
  Serial.begin(19200);
  Serial.println("Listo!");
  pinMode(ringPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ringPin, HIGH);
  digitalWrite(enablePin, LOW);
  Serial.println("RM LOW LOW LOW LOW LOW");
  analogReference(DEFAULT);
  BlinkM_begin();
  BlinkM_stopScript(0);
  BlinkM_setFadeSpeed(0,255);
}

void loop() {
  while (digitalRead(buttonPin) != HIGH){
   digitalWrite(enablePin, LOW);
   int i = analogRead(2);
//   int voiceLevel = abs(i-noiseLevel);
//   voiceLevel = map(voiceLevel, 0, 120, 0, 255);
   int voiceLevel = map(i, noiseLevel, 800, 0, 255);
   voiceLevel = constrain(voiceLevel, 0, 255);
   //int voiceLevel = map(voiceLevel, 0, 100, 0, 255);
   Serial.println(voiceLevel);
   if(voiceLevel > threshold) {
   BlinkM_fadeToHSB(0, random(0,255), 255, voiceLevel);
//   delay(10);
   }
   BlinkM_fadeToHSB(0, 0,0,idleBright);
   }
    digitalWrite(enablePin, HIGH);
//    Serial.println("Ringer on!");
    delay(10);
    ring();
    delay(ringSpacing);
//    Serial.println("Ringer off!");
}

void ring() {
  for(int i = 0; i < 5; i++) {
    //for(int ringDelay = 100; ringDelay>10; ringDelay=ringDelay-10){
    digitalWrite(ringPin, LOW);
    Serial.print("di");
    delay(ringDelay);
    digitalWrite(ringPin, HIGH);
    Serial.print("ng ");
    delay(ringDelay);
    Serial.print(ringDelay);
    Serial.print(" ");
    //} 
  }
}
