/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   


                                                                                                     
    ---What?---
    Master controller firmware for Excellent Adventure phone booths.
    See www.carbonworkshop.com/bm10 for project details.
      Monitors:
        [ADD ME]+Ambient Light
        +Switch Hook state of remote booth
        [ADD ME]+Switch Hook state of local booth
        +Audio signal from remote booth
        +Others?
      Controls:
        +LED Panels via I2C bus (using BlinkM command format)
        [ADD ME]+Warm White LED Panels via a single PWM pin
        [ADD ME]+UV LED Panels via a single PWM pin
        [ADD ME]+Red laser via a single digital pin
        [ADD ME]+Green laser via a single digital pin
        [ADD ME]+Local SLIC Enable, Ring Mode, and Forward/Reverse pins via 3 digital pins
        +Remote SLIC Enable, Ring Mode, and Forward/Reverse pins via 3 digital pins
        
    ---Why?---
    Excellent Adventure is an honorarium art installation to be deployed
    at Burning Man 2010. The project visualizes the audio signal of an
    incoming call on each of two interconnected booths on the respective
    booth's array of over 500 controlled ultra-bright RGB, warm white, 
    and UV LEDs.
    
    ---Who?---
    Daniel Massey - pichiste@gmail.com
    Ryan Alexander - scloopy@gmail.com
    Gustavo Huber - gush@carbonworkshop.com
    Gabriel Dunne - gdunne@quilime.com
    Chris C   -  bionicbadger@gmail.com
  
    ---When?---
                                                                                                 
*/

#include <math.h>
#include "Wire.h" 
#include "BlinkM_funcs.h" 

///////////////////////////////////////////


//pin config
  //Analog 0: LIGHTSENSOR
  //Analog 1: SL1_VOUT
  //Analog 2: SL2_VOUT
  //Analog 3:
  //Analog 4: Remapped to I2C SDA for LED Panels
  //Analog 5: Remapped to I2C SCK for LED Panels
  //
  //AREF:
  //GND:
  //D 13: SLI1+2 Power down
  //D 12: SL2_SHK
  //D 11 (PWM): M4
  //D 10 (PWM): M3
  //D 9 (PWM): M2
  //D 8: SL2_FR
  //
//const int   //D 7: SL2_RM
  //D 6 (PWM): M1
  //D 5 (PWM):
  //D 4: SL1_RM
  //D 3 (PWM): SL1_FR
  //D 2: SL1_SHK
  //D 1: Arduino_TX
  //D 0: Arduino_RX


//pin config
const int audioPin  = 2;  // analog audio input from the Va_OUT pin of SLIC
const int ringPin   = 2;  // F/R pin from SLIC
const int enablePin = 3;  // RM pin from SLIC
const int buttonPin = 4;  // used for testing -- triggers the ring logic pattern
const int switchHookPin = 5;  // indicates receiver is of hook when HIGH
const int voltagePin = 12;    // used for testing -- supplies voltage for buttonPin

//ringer vars
const int ringDelay = 40; //50ms = 20Hz
const int ringSpacing = 220;

//audio vars
int voiceLevel = 0;
const int zeroOffset = 505;

//blinkM addressing vars
const int  X_COUNT = 6;
const int  Y_COUNT = 11;
const int  NUM_BLINKMS = 18;
const byte FIRST_BLINKM_ADDR = 5;
//const int  X_COUNT = 2;
//const int  Y_COUNT = 10;
//const int  NUM_BLINKMS = 20;
//const byte FIRST_BLINKM_ADDR = 1;
byte addresses[X_COUNT][Y_COUNT];
byte pixelVals[X_COUNT][Y_COUNT][3];

//modes
const int IDLE = 0;
const int RINGING = 1;
const int ACTIVE = 2;
int state = IDLE;

//animations
int animationIndex = 0;
int animationCount = 1;

int switchInterval = 700;
int switchCount    = 0;


struct LedMode_testCycle
{
  void init()
  {
  };
  
  void step()
  {
    for(int y = 0; y < Y_COUNT; y++){ 
      for(int x = 0; x < X_COUNT; x++){
        setPixel(x, y, 255, 255, 0);
      }
    }
  };
} modeTestCycle;


///////////////////////////////////////////
void setup()
{
  Serial.begin(19200);
  initRinger();
  initBlinkM();
  defineBlinkMAddresses();
  initPixelVals();
  state = ACTIVE;
}

///////////////////////////////////////////
void loop()
{
  while(state == IDLE)
  {
     //do nothing... except check for ringing  
    if(digitalRead(buttonPin) == HIGH) { /* need to add conditional for whether receiver is on hook */
      state = RINGING;  
      digitalWrite(enablePin, HIGH);      
      Serial.println("ringing... ");
      delay(10);      
    }    
  }
  
  while(state == RINGING)
  {
    ring();
    delay(ringSpacing);
  
    //check to see if phone has been picked up
    if(digitalRead(switchHookPin == HIGH)) {
      state = ACTIVE;      
      digitalWrite(enablePin, LOW);   
      Serial.println("active ");   
    }
    
    //check to s=ee if other phone has been hung up
    // unimplemented...
    
    delay(ringSpacing);    
  }
  
  while(state == ACTIVE)
  {
    processAudio();   
    
//    int ai = analogRead(audioPin);
//    voiceLevel = map(ai, zeroOffset, 800, 0, 255);
//    voiceLevel = constrain(voiceLevel, 0, 255); 
//    Serial.println(voiceLevel);
      
    //update pixels here
    switch(animationIndex){
      case(0):
        modeTestCycle.step();
        break;
      case(1):
        cl_update();
        break;
      case(2):
        vs_update();
        break;
    }
    
    //send color values to BlinkMs  
    sendToBlinkMs();
    
    //necessary delay 
    delay(5);     
    
    //check to see if this phone has been hung up
    if(digitalRead(switchHookPin) == LOW) {
      voiceLevel = 0;
      BlinkM_setRGB(0, 0x00, 0x00, 0x00);      
      state = IDLE; 
      Serial.println("idle ");
    }
    
    //check if other phone has been hung up
    //unimplemented...
    
    switchCount++;
    if(switchCount >= switchInterval){
      switchCount = 0;
      animationIndex = (animationIndex + 1) % animationCount;
    }
  }
}

///////////////////////////////////////////
void ring()
{
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

///////////////////////////////////////////
void initBlinkM()
{
  BlinkM_begin();
  BlinkM_stopScript(0);  
  BlinkM_setRGB(0, 0x00, 0x00, 0x00);
}

///////////////////////////////////////////
// Maps the blinkM addresses to x, y coordinates
void defineBlinkMAddresses()
{
  byte addr = FIRST_BLINKM_ADDR;
  for(int x = 0; x < X_COUNT; x++){
    for(int y = Y_COUNT - 1; y >= 0; y--){
      addresses[x][y] = addr;
      addr++;
    }
  }
}

///////////////////////////////////////////
// Init pixel values to zero
void initPixelVals() {
  for(int x=0; x<X_COUNT; x++) {
    for(int y=0; y<Y_COUNT; y++) {
      pixelVals[x][y][0] = 0; //r
      pixelVals[x][y][1] = 1; //g
      pixelVals[x][y][2] = 2; //b
    } 
  }  
}


///////////////////////////////////////////
void sendToBlinkMs()
{
  for(int x = 0; x < X_COUNT; x++) {
    for(int y = 0; y < Y_COUNT; y++) {
      byte addr = addresses[x][y];
      BlinkM_setRGB(addr, pixelVals[x][y][0], pixelVals[x][y][1], pixelVals[x][y][2]);
    }  
  }  
}

///////////////////////////////////////////
void setPixel(int x, int y, byte r, byte g, byte b)
{
  pixelVals[x][y][0] = r;
  pixelVals[x][y][1] = g;
  pixelVals[x][y][2] = b;  
}


void initRinger()
{
  pinMode(ringPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(switchHookPin, INPUT);
  pinMode(voltagePin, OUTPUT);
  digitalWrite(ringPin, HIGH);
  digitalWrite(enablePin, LOW);
  digitalWrite(voltagePin, HIGH);
//  Serial.println("RM LOW LOW LOW LOW LOW");
  analogReference(DEFAULT); /* sets the input range for the audio. should be modified. */
}
