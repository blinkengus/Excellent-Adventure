#include <math.h>
#include "Wire.h" 
#include "BlinkM_funcs.h" 

///////////////////////////////////////////

//pin config
#include "WProgram.h"
void setup();
void loop();
void ring();
void initBlinkM();
void defineBlinkMAddresses();
void initPixelVals();
void sendToBlinkMs();
void setPixel(int x, int y, byte r, byte g, byte b);
void initRinger();
void vl_update();
float lerp(float start, float stop, float amt);
void processAudio();
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
//const int  X_COUNT = 6;
//const int  Y_COUNT = 3;
//const int  NUM_BLINKMS = 18;
//const byte FIRST_BLINKM_ADDR = 5;
const int  X_COUNT = 2;
const int  Y_COUNT = 6;
const int  NUM_BLINKMS = 12;
const byte FIRST_BLINKM_ADDR = 5;
byte addresses[X_COUNT][Y_COUNT];
byte pixelVals[X_COUNT][Y_COUNT][3];

//modes
const int IDLE = 0;
const int RINGING = 1;
const int ACTIVE = 2;
int state = IDLE;

//animations
int animationIndex = 0;


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
    vl_update();
    
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


float vl_high_row = 0;
float vl_target_row = 0;
//float vl_linger_row = 0;
//float vl_dissipation = 0.4;

int r, g, b = 0;
int bottomR = 10;
int bottomG = 0;
int bottomB = 255;
int topR = 255;
int topG = 0;
int topB = 10;


void vl_update() {
  
//  vl_high_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
  
  //determine row by lerping
  vl_target_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
  vl_high_row = round(lerp(vl_high_row, vl_target_row, 0.7));
  
//  //determine high level
//  vl_high_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
//  
//  //smooth the drop
//  if(vl_high_row < vl_linger_row) {
//    vl_linger_row = vl_high_row;  
//  } else {
//    vl_high_row = vl_linger_row;  
//  }
//  vl_linger_row += vl_dissipation;
  
  //determine color
  r = map(voiceLevel, 0, 255, bottomR, topR);
  g = map(voiceLevel, 0, 255, bottomG, topG);
  b = map(voiceLevel, 0, 255, bottomB, topB);
  
  //set color vals
  for(int y=0; y<Y_COUNT; y++) { 
    for(int x=0; x<X_COUNT; x++) {
      if(y>=(int)vl_high_row) {
        setPixel(x, y, r, g, b);
      } else {
        setPixel(x, y, 0, 0, 0);
      } 
    }
  }
  
}
/*

Some handy functions that aren't included in Arduino.

*/

float lerp(float start, float stop, float amt) {
   return start + (stop-start) * amt;
}


int lingerLevel = 0;
int dissipation = 50;
int noiseThreshold = 4;

unsigned int logTable[] = {
      65535,  65530,  65525,  65519,  65513,  65507,  65500,  65494,
      65487,  65481,  65474,  65467,  65460,  65452,  65445,  65437,
      65429,  65421,  65413,  65405,  65396,  65388,  65379,  65370,
      65360,  65351,  65341,  65331,  65321,  65311,  65300,  65289,
      65278,  65267,  65256,  65244,  65232,  65219,  65207,  65194,
      65181,  65167,  65153,  65139,  65125,  65110,  65095,  65080,
      65064,  65048,  65032,  65015,  64998,  64981,  64963,  64945,
      64926,  64907,  64888,  64868,  64847,  64827,  64805,  64784,
      64761,  64739,  64716,  64692,  64668,  64643,  64618,  64592,
      64565,  64538,  64511,  64483,  64454,  64424,  64394,  64364,
      64332,  64300,  64267,  64234,  64199,  64164,  64129,  64092,
      64055,  64016,  63977,  63937,  63897,  63855,  63812,  63769,
      63724,  63679,  63632,  63585,  63536,  63487,  63436,  63384,
      63331,  63277,  63222,  63165,  63107,  63048,  62988,  62926,
      62863,  62799,  62733,  62665,  62597,  62526,  62454,  62381,
      62306,  62229,  62151,  62071,  61989,  61905,  61820,  61732,
      61643,  61552,  61459,  61363,  61266,  61166,  61064,  60960,
      60854,  60746,  60635,  60521,  60405,  60287,  60165,  60042,
      59915,  59786,  59654,  59519,  59381,  59240,  59096,  58948,
      58798,  58644,  58486,  58326,  58162,  57994,  57822,  57647,
      57468,  57284,  57097,  56906,  56710,  56511,  56307,  56098,
      55885,  55667,  55444,  55216,  54984,  54746,  54503,  54254,
      54001,  53741,  53476,  53205,  52928,  52645,  52356,  52060,
      51758,  51450,  51134,  50812,  50482,  50145,  49801,  49449,
      49090,  48722,  48347,  47963,  47571,  47170,  46760,  46341,
      45914,  45476,  45029,  44572,  44106,  43629,  43141,  42643,
      42133,  41613,  41081,  40537,  39982,  39414,  38834,  38241,
      37634,  37015,  36382,  35735,  35074,  34398,  33707,  33001,
      32280,  31543,  30789,  30019,  29232,  28428,  27606,  26766,
      25907,  25030,  24133,  23217,  22280,  21323,  20344,  19345,
      18323,  17278,  16211,  15120,  14006,  12866,  11702,  10512,
      9296,   8053,   6783,   5485,   4158,   2802,   1416,   0
};

void processAudio() { 
  
    //read level
    int ai = analogRead(audioPin);
    
    //offset to zero and get absolute value
    voiceLevel = ai - zeroOffset;
    voiceLevel = abs(voiceLevel);
    
    //smooth
    if(voiceLevel < lingerLevel) {
      voiceLevel = lingerLevel;  
    } else {
      lingerLevel = voiceLevel;
    }    
    
    //decrement the linger level
    lingerLevel -= dissipation;    
    
    //map and constrain to 0 - 255
    voiceLevel = map(voiceLevel, noiseThreshold, 300, 0, 255);
    voiceLevel = constrain(voiceLevel, 0, 255);
   
    //get log value
    voiceLevel = logTable[255-voiceLevel]/256;
    
//    Serial.println(voiceLevel);    
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

