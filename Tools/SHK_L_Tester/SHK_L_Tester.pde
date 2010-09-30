
void setup() {
  Serial.begin(38400);
  pinMode(12, INPUT);
}

void loop() {
  int sensorValue = digitalRead(12);
  Serial.println(sensorValue, DEC);
}



