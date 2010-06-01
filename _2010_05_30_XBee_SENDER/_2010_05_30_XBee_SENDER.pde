// SENDER
//
//
//int analogValue2, analogValue5, val2, val5;
//int SwitchVal;
int SwitchPin = 10;

void setup()
{
// start serial port at 19200 bps
Serial.begin(9600);
pinMode(SwitchPin, INPUT);
pinMode(11, OUTPUT);
digitalWrite(11, HIGH);
}

void loop()
{
// read analog input
//analogValue2 = analogRead(2);
//analogValue5 = analogRead(5);
while(digitalRead(SwitchPin) == HIGH){
  Serial.println("On!");
  delay(10);
}
Serial.println("Off!");
//SwitchVal = digitalRead(SwitchPin);
// remap values

//val2 = map(analogValue2, 0, 1023, 253, 0);  // 254 and 255 for SYNC
//val5 = map(analogValue5, 0, 1023, 253, 0);

//Serial.print(255, BYTE); //SYNC char
//Serial.println(SwitchVal, BYTE);

//Serial.print(255, BYTE); //SYNC char
//Serial.print(val5, BYTE);

delay(10);
}
