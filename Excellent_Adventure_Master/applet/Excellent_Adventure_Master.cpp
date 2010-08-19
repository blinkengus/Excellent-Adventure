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
void cl_update();
void vs_update();
float lerp(float start, float stop, float amt);
void processAudio();
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
/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   


                                                                                                     
    ---What?---
    Visualization modes for Excellent Adventure booths.
    
    ---Why?---
    See Excellent Adventure Master
      
    ---Who?---
    Daniel Massey - pichiste@gmail.com
  
    ---When?---
    June 16, 2010                                                                                             
*/

//////////////////////////////////////////////////////////////////
// VERTICAL LEVEL

float vl_high_row = 0;
float vl_target_row = 0;
//float vl_linger_row = 0;
//float vl_dissipation = 0.4;

int vl_r, vl_g, vl_b = 0;
int vl_bottomR = 10;
int vl_bottomG = 0;
int vl_bottomB = 255;
int vl_topR = 255;
int vl_topG = 0;
int vl_topB = 10;


void vl_update() {
  
//  vl_high_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
  
  //determine row by lerping
  vl_target_row = map(voiceLevel, 0, 120, Y_COUNT-1, 0) + 1;
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
  vl_r = map(voiceLevel, 0, 255, vl_bottomR, vl_topR);
  vl_g = map(voiceLevel, 0, 255, vl_bottomG, vl_topG);
  vl_b = map(voiceLevel, 0, 255, vl_bottomB, vl_topB);
  
  //set color vals
  for(int y=0; y<Y_COUNT; y++) { 
    for(int x=0; x<X_COUNT; x++) {
      if(y>=(int)vl_high_row) {
        setPixel(x, y, vl_r, vl_g, vl_b);
      } else {
        setPixel(x, y, 0, 0, 0);
      } 
    }
  }
  
}

//////////////////////////////////////////////////////////////////
// TWINKLES

int cl_r, cl_g, cl_b = 0;
int cl_r1 = 0; 
int cl_g1 = 0;
int cl_b1 = 0;
int cl_r2 = 25;
int cl_g2 = 0;
int cl_b2 = 255;
float cl_angle = 0;
float cl_angle_inc;
float cl_angle_inc_min = PI/100;
float cl_angle_inc_max = PI/7;
float cl_pos = 0;


void cl_update() {
 
  float brightness = voiceLevel/255.0;
  
//  cl_angle_inc = voiceLevel*0.004 + cl_angle_inc_min;  //map(voiceLevel, 0, 255, cl_angle_inc_min, cl_angle_inc_max);
//  cl_pos = (sin(cl_angle)+1) / 2;     
//  cl_angle += cl_angle_inc;
  
//  cl_r = lerp(cl_r1, cl_r2, cl_pos);
//  cl_g = lerp(cl_g1, cl_g2, cl_pos);
//  cl_b = lerp(cl_b1, cl_b2, cl_pos); 
  
  //set color vals
  for(int y=0; y<Y_COUNT; y++) { 
    for(int x=0; x<X_COUNT; x++) {
      float localBrightness = brightness * random(1000)/1000.0;
      int r = random(255) * localBrightness;
      int g = random(255) * localBrightness;
      int b = random(255) * localBrightness;
      setPixel(x, y, r, g, b);
//      setPixel(x, y, cl_r, cl_g, cl_b);
    }
  }
  delay(50);
}

//////////////////////////////////////////////////////////////////
// VERTICAL SCANNER

int vs_row = 0;
int vs_interval;
int vs_min_interval = 2;
int vs_max_interval = 20;
int vs_counter = 0;
int vs_r = 255;
int vs_g = 255; 
int vs_b = 255;
int vs_rt = 255;
int vs_gt = 255;
int vs_bt = 255;
int vs_color_interval = 50;
int vs_color_counter = 0;

void vs_update()
{
  vs_interval = map(voiceLevel, 0, 255, vs_max_interval, vs_min_interval);
  
  if(vs_counter>=vs_interval) {
    if(vs_row>Y_COUNT-1) {
      vs_row = 0;    
    } else {
      vs_row++;  
    }
    vs_counter = 0;
  } else {
    vs_counter++;  
  }
  
  if(vs_color_counter<vs_color_interval) {
    vs_color_counter++;    
  } else {
    vs_rt = random(255);
    vs_gt = random(255);
    vs_bt = random(255);
    Serial.println(vs_r);    
    vs_color_counter = 0; 
  }
  
  //determine color
  vs_r = lerp(vs_r, vs_rt, 0.2);
  vs_g = lerp(vs_g, vs_gt, 0.2);
  vs_b = lerp(vs_b, vs_bt, 0.2);  
  
  for(int y=0; y<Y_COUNT; y++) {
    for(int x=0; x<X_COUNT; x++) {
      if(y==vs_row) {
        setPixel(x, y, vs_r, vs_g, vs_b);     
      } else {
        setPixel(x, y, 0x00, 0x00, 0x00);  
      }
    }  
  }
}

//struct LedMode_testCycle
//{
//  void init()
//  {
//  };
//  
//  void step()
//  {
//    for(int y = 0; y < Y_COUNT; y++){ 
//      for(int x = 0; x < X_COUNT; x++){
//        setPixel(x, y, 255, 255, 0);
//      }
//    }
//  };
//} modeTestCycle;
/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   


                                                                                                     
    ---What?---
    Some handy functions that are not included in Arduino by default.
    
    ---Why?---
    For linear interpolation of PWM values (colors) in Excellent Adventure.
    
    ---Who?---
    Daniel Massey - pichiste@gmail.com
      
    ---When?---
    June 19, 2010                                                                                             
*/

float lerp(float start, float stop, float amt) {
   return start + (stop-start) * amt;
}
/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   


                                                                                                     
    ---What?---
    Audio signal processing utilities for Ex-Adv Master sketch, includes
    luminance map table courtesy of John Laur - johnl@blurbco.com
    
    ---Why?---
    Because luminance is not linearly related to voltage, or in this case
    PWM duty cycle. John's table is intended for the 16-bit PWM values
    used on MaxMs running CYZ_RGB firmware, so we divide them down as
    needed.
      
    ---Who?---
    Daniel Massey - pichiste@gmail.com
    Gustavo Huber - gush@carbonworkshop.com
  
    ---When?---
    June 19, 2010                                                                                             
*/

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
    voiceLevel = analogRead(audioPin);
    
    //offset to zero and get absolute value
    voiceLevel = voiceLevel - zeroOffset;
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

