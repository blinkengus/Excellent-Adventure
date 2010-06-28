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
    Gabriel Dunne - gdunne@quilime.com
    Gustavo Huber - gush@carbonworkshop.com
  
    ---When?---
                                                                                                 
*/

#include <math.h>
#include "Wire.h" 
#include "BlinkM_funcs.h" 

///////////////////////////////////////////

//pin config
const int audioPin  = 2;
const int ringPin   = 2;
const int enablePin = 3;
const int buttonPin = 4;
const int switchHookPin = 5;
const int voltagePin = 12;

//ringer vars
const int ringDelay = 40; //50ms = 20Hz
const int ringSpacing = 220;

//audio vars
int voiceLevel = 0;
const int zeroOffset = 505;

//blinkM addressing vars
const int  X_COUNT = 6;
const int  Y_COUNT = 3;
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
int animationCount = 3;

int switchInterval = 700;
int switchCount = 0;


///////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  initRinger();
  initBlinkM(); 
  defineBlinkMAddresses();
  initPixelVals();    
}

///////////////////////////////////////////
void loop() {
  
  while(state==IDLE) {
     //do nothing... except check for ringing  
    if(digitalRead(buttonPin) == HIGH) { /* need to add conditional for whether receiver is on hook */
      state = RINGING;  
      digitalWrite(enablePin, HIGH);      
      Serial.println("ringing... ");
      delay(10);      
    }    
  }
  
  while(state==RINGING) {
    
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
  
  while(state==ACTIVE) {
    
    processAudio();   
    
//    int ai = analogRead(audioPin);
//    voiceLevel = map(ai, zeroOffset, 800, 0, 255);
//    voiceLevel = constrain(voiceLevel, 0, 255); 
//    Serial.println(voiceLevel);
      
    //update pixels here
    switch(animationIndex) {
      case(0): 
        vl_update();
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
    if(switchCount>=switchInterval) {
      switchCount = 0;
      animationIndex++;
      if(animationIndex>=animationCount) {
        animationIndex = 0;  
      }
    }
  }
  
  
  
}

///////////////////////////////////////////
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

///////////////////////////////////////////
void initBlinkM() {
  BlinkM_begin();
  BlinkM_stopScript(0);  
  BlinkM_setRGB(0, 0x00, 0x00, 0x00);
}

///////////////////////////////////////////
// Maps the blinkM addresses to x, y coordinates
void defineBlinkMAddresses() {
  byte addr = FIRST_BLINKM_ADDR;
  for(int x=0; x<X_COUNT; x++) {
    for(int y=Y_COUNT-1; y>=0; y--) {
      addresses[x][y] = addr;
      addr++; 
    }  
  }
//  addresses[0][0] = 18;
//  addresses[0][1] = 17;
//  addresses[0][2] = 16;
//  addresses[0][3] = 15;
//  addresses[0][4] = 20;
//  addresses[0][5] = 19;
//  addresses[0][6] = 14;
//  addresses[0][7] = 13;
//  addresses[0][8] = 12;
//  addresses[0][9] = 11;
//  addresses[1][0] = 8;
//  addresses[1][1] = 7;
//  addresses[1][2] = 6;
//  addresses[1][3] = 5;
//  addresses[1][4] = 10;
//  addresses[1][5] = 9;
//  addresses[1][6] = 4;
//  addresses[1][7] = 3;
//  addresses[1][8] = 2;
//  addresses[1][9] = 1;
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

void sendToBlinkMs() {
  for(int x=0; x<X_COUNT; x++) {
    for(int y=0; y<Y_COUNT; y++) {
      byte addr = addresses[x][y];
      BlinkM_setRGB(addr, pixelVals[x][y][0], pixelVals[x][y][1], pixelVals[x][y][2]);
    }  
  }  
}

///////////////////////////////////////////
void setPixel(int x, int y, byte r, byte g, byte b) {
  pixelVals[x][y][0] = r;
  pixelVals[x][y][1] = g;
  pixelVals[x][y][2] = b;  
}

void initRinger() {
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
