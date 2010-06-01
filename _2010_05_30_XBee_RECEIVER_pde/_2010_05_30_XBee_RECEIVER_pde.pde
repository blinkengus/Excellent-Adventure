// RECEIVER

//byte incomingByte, SensorVal;

void setup() {

// start serial port at 19200 bps
Serial.begin(9600);
//Serial.println("Ready! ");

// led pins

pinMode (10, OUTPUT);
//pinMode (6, OUTPUT);

//delay(150);

}

void loop() {
//if (Serial.available()) {   // are there any bytes available on the serial port ???

// assign bytes to the var ‘incomingByte’
//incomingByte = Serial.read();

//Serial.print(Serial.read());

// from now on is pretty clear I guess   :)

//if (Serial.read() = "On!")) {
digitalWrite(10, Serial.read());
//}
//else
//{
//  DigitalWrite (10, LOW);
//}
//}

}


