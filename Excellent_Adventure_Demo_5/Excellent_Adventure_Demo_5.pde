/*
  ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
 /\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
 \ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
  \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
   \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   
   
 ---What?---
 Excellent Adventure Test #4
 BlinkM VU fader for prototype of Excellent Adventure sound-to-light control system
 
 0.4
 -Use BlinkMs over I2C bus on Analog Pins 4(Data) and5(Clock)
 rather than directly controlling LEDs.
 -Change mic sampling function to only output the values which
 are greater than the noise level sampled during the setup()
 0.3
 -Fix negative analogRead() issue for audio input
 -Add smoothing for led brightness
 0.2
 -Balance VU meter against background noise
 0.1
 -React VU meter to input sound
 
 ---Why?---
 Fades and progressively turns on BlinkMs daisy chained on the I2C bus of an Arduino.
 Fading is in proportion to a normalized amplified audio input on analog pin 2, and turning
 on is done by having the normalized audio input cross static thresholds. The net
 effect should be that:
 1. When the audio input is silent, no BlinkMs are on.
 2. When the audio input is quiet, only the first BlinkM is on and dimly lit.
 3. When the audio input is normal, the first several BlinkMs are on and medium.
 4. When the audio input is loud, all BlinkMs are on and at max brightness.
 
 This is a proof of concept that will be distributed across several Arduinos
 in the Excellent Adventure booths, each one controlling a daisy chain of ~30 BlinkMs,
 as well as a couple "bonus points" effects such as PWM to a panel of warm white LEDs,
 UV LEDs, etc.
 
 ---Who?---
 Gustavo Huber - gush@carbonworkshop.com
 
 ---When?---
 May 8, 2010
 
 ---Notes---
 Big ups and credit to Tod E. Kurt @ ThingM.com for BlinkM design, library, and examples.
 * BlinkM connections to Arduino
 * PWR - -- gnd -- black -- Gnd
 * PWR + -- +5V -- red   -- 5V
 * I2C d -- SDA -- green -- Analog In 4
 * I2C c -- SCK -- blue  -- Analog In 5
 
 */

//Include BlinkM and referenced libraries
//BlinkM software (c) 2008 Tod E. Kurt, http://thingm.com/
#include "Wire.h" 
#include "BlinkM_funcs.h" 

const int NUM_BLINKMS = 4;
const byte FIRST_BLINKM_ADDR = 1;

// pin ids
const int PIN_INDICATOR_LED = 13; // onboard LED on when sampling for background noise 
const int PIN_MIC_INPUT = 0;// the mic comes in on pin 0 

int g_micThreshold = 0;
int g_micMax = 0;

const int NUM_SAMPLES = 10;
int g_samples[NUM_SAMPLES];
int g_currentSample = 0;

int detectPeakValue(int inputPin, int msecs, int indicatorPin) {
  digitalWrite(indicatorPin, HIGH); 

  int startMillis = millis();
  int micMax = 0;
  
  while ((millis() - startMillis) < msecs) { 
    int micValue = analogRead(inputPin); 
  
    // record the maximum sensor value 
    if (micValue > micMax) { 
      micMax = micValue; 
    } 
  }  
  
  digitalWrite(indicatorPin, LOW); 
  return micMax;
}

void setup() { 
  Serial.begin(57600); // had to go slow to avoid freezing!? why? 
  
  // reset samples
  for (int i=0; i<NUM_SAMPLES; i++) {
    g_samples[i] = 0;  
  }

  BlinkM_begin();

  pinMode(PIN_INDICATOR_LED, OUTPUT); // declare the ledPin as as OUTPUT 

  g_micThreshold = detectPeakValue(PIN_MIC_INPUT, 2000, PIN_INDICATOR_LED);
  delay(1000);
  g_micMax = detectPeakValue(PIN_MIC_INPUT, 2000, PIN_INDICATOR_LED);
  
  // output detected tholds
  Serial.print(g_micThreshold);
  Serial.print(" | ");
  Serial.print(g_micMax);

  BlinkM_stopScript(0); // turn off startup script 
} 

int average(int *values, int count) {
  int sum = 0;
  for (int i=0; i<count; i++){
    sum += values[i];
  }
  return sum/count;
}

void printSamples (int *samples, int count) {
  for (int i=0; i<count; i++) {
   Serial.print(samples[i]);
   Serial.print(" | ");
  }   
  Serial.print("\n");
}

void loop() {
  int micValue = analogRead(PIN_MIC_INPUT);
  g_samples[g_currentSample] = micValue;
  
  micValue = average(g_samples, NUM_SAMPLES);
  g_currentSample += 1;
  g_currentSample %= NUM_SAMPLES;
  
  // printSamples(g_samples, NUM_SAMPLES);
  micValue = min(micValue, g_micMax); // limit the mic value to the calibrated max 
  //  Serial.println(micValue);
  int numLedsOn = map(micValue, g_micThreshold, g_micMax, 0, 4);

  for (int i=0; i<=NUM_BLINKMS; i++) {
    if (i < numLedsOn) {
      BlinkM_setRGB(i + FIRST_BLINKM_ADDR, 255, 255, 255);
    } else {
      BlinkM_setRGB(i + FIRST_BLINKM_ADDR, 0, 0, 0); // fade to black  
    }
  }
    
  delay(100); 
}


