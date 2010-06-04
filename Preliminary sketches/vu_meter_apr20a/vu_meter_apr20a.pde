
//defines
#define NUMBER_OF_LEDS 3
#define NUMBER_OF_SAMPLES 20

#define DEBUG true

#define VU_METER_DISPLAY_DELAY 10

//analog in pins
byte voltageReferencePin = 0;
byte voltagePin = 2;

//digital out pins
//store the leds in an array, this way changing them later wont be a hazzle
byte ledPins[NUMBER_OF_LEDS] = { 9 , 10 , 11 };

//convert analogRead to VU ledscale
byte voltageComparisonThresholds[] = { 0 , 10 , 30 , 55 , 80 , 100 };

int voltageReference = 0; //contain analogRead(voltageReferencePin);

//sample variables
int samples[NUMBER_OF_SAMPLES] = {0};
int sample = 0;
int sampleTotal = 0;
byte sampleIndex  = 0;


void setup() {
 for (byte i=0; i<NUMBER_OF_LEDS; i++){
   pinMode(ledPins[i],OUTPUT);
 }
 pinMode(voltageReferencePin,INPUT);
 pinMode(voltagePin,INPUT);
 
 if(DEBUG){Serial.begin(9600);}
}

void loop(){
 //SMOOTH See Examples->Analog->Smoothing
 sampleTotal -= samples[sampleIndex];
 samples[sampleIndex] = analogRead(voltageReferencePin);//read value
 sampleTotal += samples[sampleIndex++];  
 if (sampleIndex >= NUMBER_OF_SAMPLES) {sampleIndex = 0;}
 sample = sampleTotal / NUMBER_OF_SAMPLES;

 if(DEBUG){Serial.print("virtual vu: ");}
 
 //DISPLAY_RESULT
 //see Examples->Digital->Loop
 for (byte i=0; i<NUMBER_OF_LEDS; i++){
   if ( analogRead(voltagePin) >= voltageReference + voltageComparisonThresholds[i] ) {
     digitalWrite(ledPins[i],HIGH); if(DEBUG){Serial.print("|");}
   }else{
     digitalWrite(ledPins[i],LOW);  if(DEBUG){Serial.print(" ");}
   }
   delay(VU_METER_DISPLAY_DELAY);
 }

 if(DEBUG){ Serial.println(" "); }
 
} 
