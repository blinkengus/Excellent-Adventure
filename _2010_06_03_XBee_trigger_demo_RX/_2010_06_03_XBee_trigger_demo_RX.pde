const int ledpin = 13;

void setup() {
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  startComms();
}

void loop() {
  sample();
  blinky(); 
}

void startComms() {
  while(Serial.available() <=0) {
    Serial.print('A', BYTE);
    delay(300);
  }
}

void sample() {
  while(Serial.available() > 0) {
    int remoteVal = Serial.read();
//    delay(10);
  }
}

void blinky() {
  digitalWrite(ledPin, HIGH);
  delay(remoteVal);
  digitalWrite(ledPin, LOW);
  delay(remoteVal);
}
  
    
