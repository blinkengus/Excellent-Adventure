void setup() {
 Serial.begin(9600);
 analogReference(INTERNAL);
 }

 void loop() {
  Serial.print(analogRead(2));
  Serial.print(" / ");
  Serial.println(analogRead(3));
  delay(50);
 }
