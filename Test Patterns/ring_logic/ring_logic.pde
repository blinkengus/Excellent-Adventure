//pin definitions
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
const byte SH2 = 12; //D 12: SL2_SHK
//D 11 (PWM): M4
//D 10 (PWM): M3
//D 9 (PWM): M2
const byte FR2 = 8;  //D 8: SL2_FR
//
const byte RM2 = 7;  //D 7: SL2_RM
//D 6 (PWM): M1
//D 5 (PWM):
const byte RM1 = 4;  //D 4: SL1_RM
const byte FR1 = 3;  //D 3 (PWM): SL1_FR
const byte SH1 = 2;  //D 2: SL1_SHK
//D 1: Arduino_TX
//D 0: Arduino_RX

const int ringdelay = 50;          //sets spacing between polarity reversals to make ring square wave, 20Hz (50ms) is standard
const int ringdelaylong = 500;     //sets spacing between ring loop sets, 500ms is standard
const byte breakringcount = 10;    //sets how many times to reverse polarity in one ring loop set
const byte maxringcount = 10;      //sets how many times a phone can be made to ring before the system times out
int globalringcount = 0;           //used to track how many times a phone has rung without someone picking it up
byte ring1counter = 0;             //used to track how many times the ring loop has been run through in a given set on phone 1
byte ring2counter = 0;             //used to track how many times the ring loop has been run through in a given set on phone 1 
byte ringdisable = 0;              //used to disable ringinging logic if both phones are not starting from an on-hook condition

void setup(){
  //SLIC 2 GPIOs
  pinMode(SH2, INPUT); 
  pinMode(FR2, OUTPUT);
  pinMode(RM2, OUTPUT);
  //SLIC 1 GPIOs
  pinMode(RM1, OUTPUT);
  pinMode(FR1, OUTPUT);
  pinMode(SH1, INPUT);
  //SLIC 1 + 2 startup logic
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, LOW);
  digitalWrite(FR1, HIGH);
  digitalWrite(FR2, HIGH);
  //Start serial for debugging
  Serial.begin(9600);
  Serial.println("Listo!");
}
//Ring loop for phone 1
void ring1(){
  Serial.println("Ring #1");
  if(ring1counter > breakringcount){  //toggle FR quickly breakringcount times, in else{} below
    ring1counter = 0;
    Serial.print("Ring 1 Pause : ");
    delay(ringdelaylong);             //after breakringcount quick rings, insert a longer delay
    globalringcount++;                //count how many ring loops have been cycled through
    Serial.println(globalringcount);  
  }
  else{
    digitalWrite(RM1, HIGH);          //enable ring mode (high voltage on Ring/Tip)
    delay(ringdelay);                 //pause a bit to establish top half of ring waveform
    digitalWrite(FR1, LOW);           //reverse Ring/Tip polarity
    delay(ringdelay);                 //pause a bit to establish bottom half of ring waveform
    digitalWrite(FR1, HIGH);          //set normal Ring/Tip polarity
    digitalWrite(RM1, LOW);           //disable ring mode 
    ring1counter++;                    
  }
}
//Ring loop for phone 2
void ring2(){
  Serial.println("Ring #2");
  if(ring2counter > breakringcount){
    ring2counter = 0;
    Serial.print("Ring 2 Pause : ");
    delay(ringdelaylong);
    globalringcount++;
    Serial.println(globalringcount);
  }
  else{
    digitalWrite(RM2, HIGH);
    delay(ringdelay);
    digitalWrite(FR2, LOW);
    delay(ringdelay);
    digitalWrite(FR2, HIGH);
    digitalWrite(RM2, LOW);
    ring2counter++;
  }
}

void loop(){
  int SH1state = digitalRead(SH1);
  int SH2state = digitalRead(SH2);
  
  //If phone 1 is off the hook, and phone 2 is on the hook, and both were on the hook the last time we sampled, ring phone 2
  if(SH1state > SH2state && ringdisable == 0 && globalringcount < maxringcount){
    ring2();
  }
  //If phone 2 is off the hook, and phone 1 is on the hook, and both were on the hook the last time we sampled, ring phone 1
  else if(SH1state < SH2state && ringdisable == 0 && globalringcount < maxringcount){
    ring1();
  }
  //If both phones are off the hook, don't ring either, and disable ringing until both are hung up
  else if(SH1state + SH2state == 2){
    Serial.println("Ringer disabled");
    ringdisable = 1;
  }
  //If both phones are on the hook, don't ring either, but remove the ringdisable var, and reset the ring counter
  else if(SH1state + SH2state == 0){
    ringdisable = 0;
    globalringcount = 0;
    Serial.println(" --- ");
  }
  //If one phone is on the hook, and the other is off the hook, but the number of ring loops has exceed the maxringcount variable, stop ringing it
  else{
    Serial.println("TOO MANY RINGS! HANG UP A PHONE!");
  }
}


