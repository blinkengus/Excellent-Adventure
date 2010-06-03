/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   

    ---What?---
    Excellent Adventure Test #3
    LED VU fader for prototype of Excellent Adventure sound-to-light control system
    
    0.3
    -Fix negative analogRead() issue for audio input
    -Add smoothing for led brightness
    0.2
    -Balance VU meter against background noise
    0.1
    -React VU meter to input sound
    
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
    May 1, 2010
                                                                                                     
*/

//definitions
#define no_LED 6 //number of LEDs -- only 6 PWM slots available
#define no_Samples 50 //number of samples taken to determine a stable voltage reading -- not sure about this one

#define DEBUG true //set to true if you want serial printouts for the relevant values

#define VU_delay 0

//audio inputs
int audio_ref = 0;
int audio_in = 2;

//LED outputs
int LED[no_LED] = { 3 , 5 , 6 , 9 , 10 , 11 }; //this array can only be PWM pins, all 6 included here (3,5,6,9,10,11)

//set thresholds for the VU meter
int voltageComparisonThresholds[] = { 50 , 100 , 130 , 225 , 300 , 360 };
//int voltageReference = 0; //contain analogRead(audio_ref);

//sample variables
int rawIn = 0;
int Level = 0;
int cleanLevel = 0;
int cleanLight = 0;
int sampleTotal = 0;
int sampleIndex  = 0;

//SMOOTHING for brightness
const int numReadings = 50;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int inputPin = audio_in;


void setup() {
 for (int i=0; i<no_LED; i++){
   pinMode(LED[i],OUTPUT);
 }
 pinMode(audio_ref,INPUT);
 pinMode(audio_in,INPUT);
 
 if(DEBUG){Serial.begin(9600);}

  // SMOOTHING: initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  readings[thisReading] = 0;          
}

void loop(){

//SMOOTHING Loop
  // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(inputPin); 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings;               
  
//sound stuff
 rawIn = analogRead(audio_in);  
  
 //grab the input level
 Level = rawIn;

 //offset to correct correct 'zero' level
 cleanLevel = ( Level >= 512 ) ? Level - 512 : 512 - Level;  
 
 //map level to correct range
 int cleanLight = map(average, 512, 1024, 0, 255);//map(cleanLevel, 0, 1024, 0, 255);
 cleanLight = constrain(cleanLight, 0, 255); 
 
 if(DEBUG){
    Serial.print(" Vin: ");
    Serial.print(rawIn);
    Serial.print(" Level: ");
    Serial.print(Level);
    Serial.print(" CleanLevel: ");
    Serial.print(cleanLevel);
    Serial.print(" average: ");
    Serial.print(average);
    Serial.print(" CleanLight: ");
    Serial.print(cleanLight);
    Serial.print(" / VU: ");
  }
 
 //DISPLAY_RESULT
 
 for (int i=0; i<no_LED; i++){
   if ( cleanLevel >= voltageComparisonThresholds[i] ) {
       analogWrite(LED[i], cleanLight); if(DEBUG){Serial.print("O");}
   }else{
     analogWrite(LED[i], 0);  if(DEBUG){Serial.print(" ");}
   }
   delay(VU_delay);
 }
// for (int i=0; i<no_LED; i++) {
//   analogWrite(LED[i], cleanLight);  
// }

 if(DEBUG){ Serial.println(" "); }
 
} 

