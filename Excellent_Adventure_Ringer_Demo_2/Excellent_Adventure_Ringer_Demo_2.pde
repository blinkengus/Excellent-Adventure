/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _/\ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   

                                                                                 
    ---What?---
    Excellent Adventure Ringer Trigger Demo #2
    Prototype of Ringer activation through XBee and Solarbotics Stealth Motor Driver.
    
    ---Why?---
    So Arduino can be remotely triggered to ring the regular phone ringer, even though arduino only
    works in DC.
    
    ---Who?---
    Gustavo Huber - gush@carbonworkshop.com
  
    ---When?---
    May 13, 2010                                                                                                 
*/
int buttonstate = 0;

void setup(){
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(13, INPUT);
}

void loop(){
  buttonstate = digitalRead(13);
  while(buttonstate == HIGH){
  digitalWrite(3, LOW);
  digitalWrite(2, HIGH);
  delay(50);

  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  delay(50);
  buttonstate = digitalRead(13);
  }
}
