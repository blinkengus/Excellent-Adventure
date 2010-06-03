/*
 ______     ______     __   __     __     __     ______     __  __     ______     __  __     ______  
/\  ___\   /\  == \   /\ "-.\ \   /\ \  _ \ \   /\  == \   /\ \/ /    /\  ___\   /\ \_\ \   /\  == \ 
\ \ \____  \ \  __<   \ \ \-.  \  \ \ \/ ".\ \  \ \  __<   \ \  _"-.  \ \___  \  \ \  __ \  \ \  _-/ 
 \ \_____\  \ \_____\  \ \_\\"\_\  \ \__/".~\_\  \ \_\ \_\  \ \_\ \_\  \/\_____\  \ \_\ \_\  \ \_\   
  \/_____/   \/_____/   \/_/ \/_/   \/_/   \/_/   \/_/ /_/   \/_/\/_/   \/_____/   \/_/\/_/   \/_/   
                                                                                                     
    ---What?---
    Excellent Adventure Test #2
    LED VU fader for prototype of Excellent Adventure sound-to-light control system
    
    ---Why?---
    Fades and progressively turns on PWM outputs of an Arduino Diecimilia. Fading is
    in proportion to a normalized amplified audio input on analog pin 2, and turning
    on is done by having the normalized audio input cross static thresholds. The net
    effect should be that:
    1. When the audio input is silent, no LEDs are on.
    2. When the audio input is quiet, only the first LED is on and dimly lit.
    3. When the audio input is normal, the first several LEDs are on and medium.
    4. When the audio input is loud, all LEDs are on and at max brightness.
    
    This is a proof of concept that will be distributed across several Arduinos
    in the Excellent Adventure booths -- possibly one uC per LED color channel.
      
    ---Who?---
    Gustavo Huber - gush@carbonworkshop.com
  
    ---When?---
    April 22, 2010
                                                                                                     
*/

//definitions
#define no_LED 6 //number of LEDs -- only 6 PWM slots available
#define no_Samples 100 //number of samples taken to determine a stable voltage reading -- not sure about this one

#define DEBUG true //set to true if you want serial printouts for the relevant values

#define VU_delay 0

//audio inputs
#include "WProgram.h"
void setup();
void loop();
int audio_ref = 0;
int audio_in = 2;

//LED outputs
int LED[no_LED] = { 3 , 5 , 6 , 9 , 10 , 11 }; //this array can only be PWM pins, all 6 included here (3,5,6,9,10,11)

//set thresholds for the VU meter
int voltageComparisonThresholds[] = { 50 , 100 , 130 , 225 , 300 , 360 };

int voltageReference = 0; //contain analogRead(audio_ref);

//sample variables
int samples[no_Samples] = {0};
int sample = 0;
int noiseLevel = 450;
int Level = 0;
int cleanLevel = 0;
int cleanLight = 0;
int sampleTotal = 0;
int sampleIndex  = 0;


void setup() {
 for (int i=0; i<no_LED; i++){
   pinMode(LED[i],OUTPUT);
 }
 pinMode(audio_ref,INPUT);
 pinMode(audio_in,INPUT);
 
 if(DEBUG){Serial.begin(9600);} //serial set to 9600bps
}

void loop(){

 //smooth the analog values from the audio input
 sampleTotal -= samples[sampleIndex];
 samples[sampleIndex] = analogRead(audio_ref);//read value
 sampleTotal += samples[sampleIndex++];  
 if (sampleIndex >= no_Samples) {sampleIndex = 0;}
 sample = sampleTotal / no_Samples;
 Level = analogRead(audio_in) - noiseLevel; //the circuit introduces about 30% noise, so this is just setting a base level by experimentation
 cleanLevel = abs(Level); //microphones are AC, so this is a hack to deal with the negative values that are rendered as the diaphragm relaxes
 int cleanLight = map(cleanLevel, 0, 1024, 0, 254); //remap the audio volume to the range suitable for PWM output
 
 if(DEBUG){
    Serial.print("Vref: ");
    Serial.print(analogRead(audio_ref));
    Serial.print(" Vin: ");
    Serial.print(analogRead(audio_in));
    Serial.print(" CleanLevel: ");
    Serial.print(cleanLevel);
    Serial.print(" CleanLight: ");
    Serial.print(cleanLight);
  //  Serial.print(" Sample: ");
  //  Serial.print(sample);
    Serial.print(" / VU: ");}
 
 //DISPLAY_RESULT
 for (int i=0; i<no_LED; i++){
   if ( cleanLevel >= voltageComparisonThresholds[i] ) {
       analogWrite(LED[i], cleanLight); if(DEBUG){Serial.print("O");}
   }else{
     analogWrite(LED[i], 0);  if(DEBUG){Serial.print(" ");}
   }
   delay(VU_delay);
 }

 if(DEBUG){ Serial.println(" "); }
 
} 

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

