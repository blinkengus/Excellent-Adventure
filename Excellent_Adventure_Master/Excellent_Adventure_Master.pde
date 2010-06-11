/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   


                                                                                                     
    ---What?---
    Combined "master" firmware for Excellent Adventure.
    Controls LED panels, dumb LEDs, SLIC, and takes analog
    sound and ambient light inputs.
    
    ---Why?---
    www.carbonworkshop.com/bm10
      
    ---Who?---
    Gustavo Huber - gush@carbonworkshop.com
    Daniel Massey - pichiste@gmail.com
  
    ---When?---
    June 11, 2010                                                                                             
*/

#include "Wire.h" 
#include "BlinkM_funcs.h" //try to use the actual wiring code so we can omit this library in the final build

///////////////////////////////////////////
//ringer vars
const int ringPin   = 2;
const int enablePin = 3;
const int buttonPin = 4;
const int switchHookPin = 5;
const int ringDelay = 40; //50ms = 20Hz
const int ringSpacing = 220;

//audio vars
const int audioPin = 2;
int voiceLevel = 0;
const int noiseLevel = 505;
//const int threshold = 0;
const int idleBright = 0;

//blinkM addressing vars
const int  X_COUNT = 6;
const int  Y_COUNT = 3;
const int  NUM_BLINKMS = 18;
const byte FIRST_BLINKM_ADDR = 5;
byte addresses[X_COUNT][Y_COUNT];
byte pixelVals[X_COUNT][Y_COUNT][3];

//modes
const int IDLE = 0;
const int RINGING = 1;
const int ACTIVE = 2;
int state = IDLE;


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
    if(digitalRead(buttonPin)==HIGH ) { /* need to add conditional for whether receiver is on hook */
      state = RINGING;  
      digitalWrite(enablePin, HIGH);      
      delay(10);    
      Serial.println("change to ringing mode" );  
    }    
  }
  
  while(state==RINGING) {
    
    ring();
    delay(ringSpacing);
  
    //check to see if phone has been picked up
    if(digitalRead(switchHookPin) == HIGH) {
      state = ACTIVE;      
      digitalWrite(enablePin, LOW);   
      Serial.println("change to active mode");   
    }
    
    //check to see if other phone has been hung up
    // unimplemented...
    
    delay(ringSpacing);    
  }
  
  while(state==ACTIVE) {
    
    //process audio  
    int ai = analogRead(audioPin);
    voiceLevel = map(ai, noiseLevel, 800, 0, 255);
    voiceLevel = constrain(voiceLevel, 0, 255);    
      
    //update pixels here, in dan's sketch
    d_update();
    
    
    //send color values to BlinkMs  
    for(int x=0; x<X_COUNT; x++) {
      for(int y=0; y<Y_COUNT; y++) {
        byte addr = addresses[x][y];
        BlinkM_setRGB(addr, pixelVals[x][y][0], pixelVals[x][y][1], pixelVals[x][y][2]);
      }  
    }
    delay(5);     
    
    //check to see if this phone has been hung up
    if(digitalRead(switchHookPin) == LOW) {
      state = IDLE; 
      Serial.println("change to idle mode");
    }
    
    //check if other phone has been hung up
    //unimplemented...
    
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
  Wire.begin();  //replaces BlinkM_begin()
//  BlinkM_begin(); 
//  BlinkM_stopScript(0);
Wire.beginTransmission(0); //replaces BlinkM_stopScript(0)
  Wire.send('o');
  Wire.endTransmission();  
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
  digitalWrite(ringPin, HIGH);
  digitalWrite(enablePin, LOW);
  Serial.println("RM LOW LOW LOW LOW LOW");
  analogReference(DEFAULT);  
}
