/*
 * Blinky Panel Scanner -- 
 *
 *
 * Blinky Panel connections to Arduino
 * PWR - -- gnd -- black -- Gnd
 * PWR + -- +5V -- red   -- 5V
 * I2C d -- SDA -- green -- Analog In 4
 * I2C c -- SCK -- blue  -- Analog In 5
 *
 *
 * 2007, Tod E. Kurt, ThingM, http://thingm.com/
 *
 */

// Adapted from
//http://code.google.com/p/arduino/issues/attachmentText?id=28&aid=-2270486327270254489&name=Wire.diff&token=9f437f2c4975f2eb605ac29648a7885e

#include "Wire.h"
#include "BlinkM_funcs.h"

//fast I2C defs
#define I2C_FAST 100000L
#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))

//globals
#include "WProgram.h"
long unsigned int twi_setSpeed(long unsigned int bitsPerSecond);
void setup();
void loop();
void panelstepper();
void speedypanelstepper();
void rowchaser();
void columnchaser();
void columnspin();
void rowsweep();
void corkscrew();
void colorsweeps();
void colorfades();
void strobemodes();
void randomcolors_accelerate();
void randomcolors_decelerate();
void whitefadeout();
void redfadeout();
void greenfadeout();
void bluefadeout();
void cyanfadeout();
void magentafadeout();
void yellowfadeout();
void whitestrobe();
void redstrobe();
void greenstrobe();
void bluestrobe();
void cyanstrobe();
void magentastrobe();
void yellowstrobe();
void colorstrobe();
const int commandrepeats = 1; // how many times to repeat each I2C command to avoid dropped packets (6 seems to be the sweet spot)
const int firstPanel = 10;
const int lastPanel = 99;
const int columncount = 9;
const int rowcount = 10;
const int maxwhite = 235; //highest possible value when setting R,G,B equal each other (limit to 230 when using 10A power supply, 215 when using deep-cycle 6v battery [225 with a supercap pack])
const int mincolor = 0; // if colors seem too dim too often, mess with this
const int maxcolor = 255; // highest possible value when setting a color (limit to 240 when using 10A power supply, 235 when using a 6v deep cycle battery [245 with a supercap pack])
const int stepcycles = 1; // how many times each effect runs

//for fullspeedpanelstepper() mode
const int speedystepcycles = 10; //how many times to loop through all 100 panels on the speed test
const int speedystepdelay = 0; //leave at 0 for speed test

//for rowchaser() and columnchaser() modes
const int chasedelay = 75;
const int chasefadeinspeed = 50;
const int chasefadeoutspeed = 3;
const int sweepfadespeed = 2;

//for rowsweep() mode
const int rowsweepdelay = 75;
const int sweepfadeinspeed = 20;
const int sweepfadeoutspeed = 5;

//for columnspin() mode
const int spindelay = 75;
const int spinfadeinspeed = 20;
const int spinfadeoutspeed = 5;

//for corkscrew() mode
const int screwdelay = 25;
const int screwfadeinspeed = 40;
const int screwfadeoutspeed = 15;
const int screwoffset = 8;

//for panelstepper() mode
const int stepfadespeed = 5;
const int stepdelay = 50;

//for strobemodes()
const int strobewhitedelay = 30; //(aggressive = 30)
const int strobeblackdelay = 150; //(aggressive = 150)
const int strobecount = 3; //(about 5-6 can occur per second)
const int colorstrobecount = 6;

//for colorsweeps() mode
const int randomcolorcount = 10;
const int sweepdelay = randomcolorcount;

//for colorfades() mode
const int fadedelay = 250; //(1500 smooth and slow)
const int fadespeed = 6;  //(3 smooth and slow)

//vars
int r = 0;
int b = 0;
int g = 0;
int commandrepeatcounter = 0;

//FAST I2C
long unsigned int twi_setSpeed(long unsigned int bitsPerSecond)
{
 /* twi bit rate formula from atmega128 manual pg 204
 SCL Frequency = CPU Clock Frequency / (16 + (TWBR * 2 * prescale))
 NOTE: TWBR should be 10 or higher for master mode
 It is 72 for a 16mhz Wiring board with 100kHz TWI */

 // calculate the former bit rate to return
 uint8_t prescale = 1;
 if (TWSR & _BV(TWPS0)) { prescale = prescale << 2; }
 if (TWSR & _BV(TWPS1)) { prescale = prescale << 4; }
 long unsigned int formerSpeed = (F_CPU / (16 + (TWBR * 2 * prescale)));

 // initialize twi prescaler and bit rate
 cbi(TWSR, TWPS0);  // both bits cleared yields prescaler value of 1
 cbi(TWSR, TWPS1);
 TWBR = ((F_CPU / bitsPerSecond) - 16) / 2;

 return formerSpeed;
 }

void setup()
{
  BlinkM_begin();
  //BlinkM_beginWithPower();
  randomSeed(analogRead(0));
  //Serial.begin(19200);
  //BlinkM_stopScript(0);
  //BlinkM_setFadeSpeed(0, 50);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setRGB(0, 0x00, 0x00, 0x00);
  }
  delay(250);
}


void loop() {
  digitalWrite(13, HIGH);
  delay(10);
  //panelstepper();
  //speedypanelstepper();
  //rowchaser();
  columnchaser();
  rowsweep();
  //columnspin();
  //corkscrew();
  //colorsweeps();
  //colorfades();
  //strobemodes();
  //delay(100);
  digitalWrite(13, LOW);
  delay(10);
}

void panelstepper(){
  for(int n = 0; n < stepcycles; n++){
    r = random(mincolor, maxcolor);
    g = random(mincolor, maxcolor);
    b = random(mincolor, maxcolor);
    delay(stepdelay*10);
    for(int i = firstPanel; i < (lastPanel+1); i++){
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
        //BlinkM_setFadeSpeed(i, stepfadespeed);
        //BlinkM_fadeToRGB(i, (byte)r, (byte)g, (byte)b);
      }
      //      Serial.print(i); 
      //      Serial.print(":   ");
      //      Serial.print(r); 
      //     Serial.print(" / "); 
      //      Serial.print(g); 
      //      Serial.print(" / "); 
      //      Serial.println(b);
      delay(stepdelay);
    }
    for (int j = firstPanel; j < (lastPanel+1); j++){
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        BlinkM_setRGB(j, 0x00, 0x00, 0x00);
        //BlinkM_setFadeSpeed(j, stepfadespeed);
        //BlinkM_fadeToRGB(j, 0x00, 0x00, 0x00);
      }
      //      Serial.print(j); 
      //      Serial.print(":   ");
      //      Serial.print(r); 
      //      Serial.print(" / "); 
      //      Serial.print(g); 
      //      Serial.print(" / "); 
      //      Serial.println(b);
      delay(stepdelay);

    }
  }
}

void speedypanelstepper(){
  for(int n = 0; n < speedystepcycles; n++){
    r = random(mincolor, maxcolor);
    g = random(mincolor, maxcolor);
    b = random(mincolor, maxcolor);
    delay(speedystepdelay);
    for(int i = firstPanel; i < (lastPanel+1); i++){
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
        //BlinkM_setFadeSpeed(i, stepfadespeed);
        //BlinkM_fadeToRGB(i, (byte)r, (byte)g, (byte)b);
      }
      //      Serial.print(i); 
      //      Serial.print(":   ");
      //      Serial.print(r); 
      //     Serial.print(" / "); 
      //      Serial.print(g); 
      //      Serial.print(" / "); 
      //      Serial.println(b);
      delay(speedystepdelay);
    }
    for (int j = firstPanel; j < (lastPanel+1); j++){
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        BlinkM_setRGB(j, 0x00, 0x00, 0x00);
        //BlinkM_setFadeSpeed(j, stepfadespeed);
        //BlinkM_fadeToRGB(j, 0x00, 0x00, 0x00);
      }
      //      Serial.print(j); 
      //      Serial.print(":   ");
      //      Serial.print(r); 
      //      Serial.print(" / "); 
      //      Serial.print(g); 
      //      Serial.print(" / "); 
      //      Serial.println(b);
      delay(speedystepdelay);

    }
  }
}

void rowchaser(){
  for(int n = 0; n < stepcycles; n++){
    int r = random(mincolor, maxcolor);
    int g = random(mincolor, maxcolor);
    int b = random(mincolor, maxcolor);
    delay(stepdelay*10);
    for(int i = firstPanel; i < (lastPanel+1); i++){
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        //BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
        BlinkM_setFadeSpeed(i, chasefadeinspeed);
        BlinkM_fadeToRGB(i, (byte)r, (byte)g, (byte)b);
      }
      //      Serial.print(i); 
      //      Serial.print(":   ");
      //      Serial.print(r); 
      //      Serial.print(" / "); 
      //      Serial.print(g); 
      //      Serial.print(" / "); 
      //      Serial.println(b);
      delay(chasedelay);
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        //BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
        BlinkM_setFadeSpeed(i, chasefadeoutspeed);
        BlinkM_fadeToRGB(i, 0x00, 0x00, 0x00);
      }

      //      Serial.print(i); 
      //      Serial.print(":   ");
      //      Serial.print(r); 
      //      Serial.print(" / "); 
      //      Serial.print(g); 
      //      Serial.print(" / "); 
      //      Serial.println(b);
    }
    //delay(chasedelay);
  }
}

void columnchaser(){
  for(int n = 0; n < stepcycles; n++){
    int r = random(mincolor, maxcolor);
    int g = random(mincolor, maxcolor);
    int b = random(mincolor, maxcolor);
    delay(stepdelay*10);
    for(int col = 0; col < columncount; col++){
      for(int i = firstPanel + col; i < (lastPanel+1); i=i+9){
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
          BlinkM_setFadeSpeed(i, chasefadeinspeed);
          BlinkM_fadeToRGB(i, (byte)r, (byte)g, (byte)b);
        }
        //        Serial.print(i); 
        //        Serial.print(":   ");
        //        Serial.print(r); 
        //        Serial.print(" / "); 
        //        Serial.print(g); 
        //        Serial.print(" / "); 
        //        Serial.println(b);
        delay(chasedelay);
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
          BlinkM_setFadeSpeed(i, chasefadeoutspeed);
          BlinkM_fadeToRGB(i, 0x00, 0x00, 0x00);
        }
        //        Serial.print(i); 
        //        Serial.print(":   ");
        //        Serial.print(r); 
        //        Serial.print(" / "); 
        //        Serial.print(g); 
        //        Serial.print(" / "); 
        //        Serial.println(b);
      }

    }

    //delay(chasedelay);
  }  
}

void columnspin(){
  for(int n = 0; n < stepcycles; n++){
    int r = random(mincolor, maxcolor);
    int g = random(mincolor, maxcolor);
    int b = random(mincolor, maxcolor);
    delay(spindelay);
    for(int col = 0; col < columncount; col++){
      for(int i = firstPanel + col; i < (lastPanel+1); i=i+9){
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
          BlinkM_setFadeSpeed(i, spinfadeinspeed);
          BlinkM_fadeToRGB(i, (byte)r, (byte)g, (byte)b);
        }
        //        Serial.print(i); 
        //        Serial.print(":   ");
        //        Serial.print(r); 
        //        Serial.print(" / "); 
        //        Serial.print(g); 
        //        Serial.print(" / "); 
        //        Serial.println(b);
      }
      delay(chasedelay);
      for(int i = firstPanel + col; i < (lastPanel+1); i=i+9){
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, 0x00, 0x00, 0x00);
          BlinkM_setFadeSpeed(i, spinfadeoutspeed);
          BlinkM_fadeToRGB(i, 0x00, 0x00, 0x00);
        }
        //        Serial.print(i); 
        //        Serial.print(":   ");
        //        Serial.print(r); 
        //        Serial.print(" / "); 
        //        Serial.print(g); 
        //        Serial.print(" / "); 
        //        Serial.println(b);
      }
    }
    //delay(chasedelay);
  }  
}

void rowsweep(){
  for(int n = 0; n < stepcycles; n++){
    int r = random(mincolor, maxcolor);
    int g = random(mincolor, maxcolor);
    int b = random(mincolor, maxcolor);
    delay(rowsweepdelay);
    for(int row = 0; row < rowcount; row++){
      for(int z = 0; z < columncount; z++){
        int i = firstPanel + (9 * row) + z;
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
          BlinkM_setFadeSpeed(i, sweepfadeinspeed);
          BlinkM_fadeToRGB(i, (byte)r, (byte)g, (byte)b);
        }
        //        Serial.print(i); 
        //        Serial.print(":   ");
        //        Serial.print(r); 
        //        Serial.print(" / "); 
        //        Serial.print(g); 
        //        Serial.print(" / "); 
        //        Serial.println(b);
      }
      delay(rowsweepdelay);
      for(int z = 0; z < columncount; z++){
        int i = firstPanel + (9 * row) + z;
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, 0x00, 0x00, 0x00);
          BlinkM_setFadeSpeed(i, sweepfadeoutspeed);
          BlinkM_fadeToRGB(i, 0x00, 0x00, 0x00);
        }
      }  
    }
    //delay(chasedelay);
  }  
}

void corkscrew(){
  for(int n = 0; n < stepcycles; n++){
    int r = random(mincolor, maxcolor);
    int g = random(mincolor, maxcolor);
    int b = random(mincolor, maxcolor);
    delay(screwdelay);
    for(int col = 0; col < columncount; col++){
      for(int i = firstPanel + col; i < (lastPanel+1); i = i + screwoffset){
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, (byte)r, (byte)g, (byte)b);
          BlinkM_setFadeSpeed(i, screwfadeinspeed);
          BlinkM_fadeToRGB(i, (byte)r, (byte)g, (byte)b);
        }
        //        Serial.print(i); 
        //        Serial.print(":   ");
        //        Serial.print(r); 
        //        Serial.print(" / "); 
        //        Serial.print(g); 
        //        Serial.print(" / "); 
        //        Serial.println(b);
      }
      delay(screwdelay);
      for(int i = firstPanel + col; i < (lastPanel+1); i = i + screwoffset){
        for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
          //BlinkM_setRGB(i, 0x00, 0x00, 0x00);
          BlinkM_setFadeSpeed(i, screwfadeoutspeed);
          BlinkM_fadeToRGB(i, 0x00, 0x00, 0x00);
        }
        //        Serial.print(i); 
        //        Serial.print(":   ");
        //        Serial.print(r); 
        //        Serial.print(" / "); 
        //        Serial.print(g); 
        //        Serial.print(" / "); 
        //        Serial.println(b);
      }
    }
    //delay(screwdelay);
  }  
}

void colorsweeps(){
  randomcolors_accelerate();
  //colorstrobe();
  randomcolors_decelerate();
}

void colorfades(){
  redfadeout();
  greenfadeout();
  bluefadeout();
  cyanfadeout();
  magentafadeout();
  yellowfadeout();
  whitefadeout();
}

void strobemodes(){
  whitestrobe();
  redstrobe();
  greenstrobe();
  bluestrobe();
  cyanstrobe();
  magentastrobe();
  yellowstrobe();
  colorstrobe();
}

void randomcolors_accelerate(){
  for(int i = 0; i < randomcolorcount; i++){

    int r = random(mincolor, maxcolor);//constrain(g + 85, 0, maxwhite);//random(255);
    int g = random(mincolor, maxcolor);//constrain(b - 85, 0, maxwhite);//random(255);
    int b = random(mincolor, maxcolor);//constrain(r + 43, 0, maxwhite);//random(255);

    //BlinkM_fadeToRGB( 0, 0x00, 0x00, 0x00);

    for(int p = 1; p < 100; p ++){
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        BlinkM_setFadeSpeed( p, sweepfadespeed);
        BlinkM_fadeToRGB( p, (byte)r, (byte)g, (byte)b  );
      }
      delay(sweepdelay-i);
    }
  }
}

void randomcolors_decelerate(){
  for(int i = randomcolorcount; i < 1; i--){

    int r = random(mincolor, maxcolor);//constrain(g + 85, 0, maxwhite);//random(255);
    int g = random(mincolor, maxcolor);//constrain(b - 85, 0, maxwhite);//random(255);
    int b = random(mincolor, maxcolor);//constrain(r + 43, 0, maxwhite);//random(255);

    //BlinkM_fadeToRGB( 0, 0x00, 0x00, 0x00);

    for(int p = 1; p < 100; p ++){
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        BlinkM_setFadeSpeed( p, sweepfadespeed);
        BlinkM_fadeToRGB( p, (byte)r, (byte)g, (byte)b  );
      }
      delay(sweepdelay-i);
    }
  }
}


void whitefadeout(){
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setFadeSpeed(0, fadespeed);
    BlinkM_fadeToRGB(0, maxwhite, maxwhite, maxwhite);
  }
  delay(fadedelay);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_fadeToRGB(0, 0, 0, 0);
  }
  delay(fadedelay);
}

void redfadeout(){
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setFadeSpeed(0, fadespeed);
    BlinkM_fadeToRGB(0, maxcolor, 0, 0);
  }
  delay(fadedelay);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_fadeToRGB(0, 0, 0, 0);
  }
  delay(fadedelay);
}

void greenfadeout(){
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setFadeSpeed(0, fadespeed);
    BlinkM_fadeToRGB(0, 0, maxcolor, 0);
  }
  delay(fadedelay);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_fadeToRGB(0, 0, 0, 0);
  }
  delay(fadedelay);
}

void bluefadeout(){
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setFadeSpeed(0, fadespeed);
    BlinkM_fadeToRGB(0, 0, 0, maxcolor);
  }
  delay(fadedelay);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_fadeToRGB(0, 0, 0, 0);
  }
  delay(fadedelay);
}

void cyanfadeout(){
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setFadeSpeed(0, fadespeed);
    BlinkM_fadeToRGB(0, 0, maxcolor, maxcolor);
  }
  delay(fadedelay);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){ 
    BlinkM_fadeToRGB(0, 0, 0, 0);
  }
  delay(fadedelay);
}

void magentafadeout(){
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setFadeSpeed(0, fadespeed);
    BlinkM_fadeToRGB(0, maxcolor, 0, maxcolor);
  }
  delay(fadedelay);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_fadeToRGB(0, 0, 0, 0);
  }
  delay(fadedelay);
}

void yellowfadeout(){
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_setFadeSpeed(0, fadespeed);
    BlinkM_fadeToRGB(0, maxcolor, maxcolor, 0);
  }
  delay(fadedelay);
  for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
    BlinkM_fadeToRGB(0, 0, 0, 0);
  }
  delay(fadedelay);
}

void whitestrobe(){
  //  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, maxwhite, maxwhite, maxwhite);
    }
    delay(strobewhitedelay);
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, 0);
    }
    delay(strobeblackdelay);
  }
}

void redstrobe(){
  //  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, maxcolor, 0, 0);
    }
    delay(strobewhitedelay);
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, 0);
    }
    delay(strobeblackdelay);
  }
}

void greenstrobe(){
  //  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, maxcolor, 0);
    }
    delay(strobewhitedelay);
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, 0);
    }
    delay(strobeblackdelay);
  }
}

void bluestrobe(){
  //  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, maxcolor);
    }
    delay(strobewhitedelay);
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, 0);
    }
    delay(strobeblackdelay);
  }
}

void cyanstrobe(){
  //  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, maxcolor, maxcolor);
    }
    delay(strobewhitedelay);
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, 0);
    }
    delay(strobeblackdelay);
  }
}

void magentastrobe(){
  //  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, maxcolor, 0, maxcolor);
    }
    delay(strobewhitedelay);
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, 0);
    }
    delay(strobeblackdelay);
  }
}

void yellowstrobe(){
  //  BlinkM_setFadeSpeed(0, 255);
  for(int i = 0; i < strobecount; i++){
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, maxcolor, maxcolor, 0);
    }
    delay(strobewhitedelay);
    for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
      BlinkM_setRGB(0, 0, 0, 0);
    }
    delay(strobeblackdelay);
  }
}

void colorstrobe(){
  for(int i = 0; i < colorstrobecount; i++){

    int r = random(mincolor, maxcolor);//constrain(g + 85, 0, maxwhite);//random(255);
    int g = random(mincolor, maxcolor);//constrain(b - 85, 0, maxwhite);//random(255);
    int b = random(mincolor, maxcolor);//constrain(r + 43, 0, maxwhite);//random(255);

    //BlinkM_fadeToRGB( 0, 0x00, 0x00, 0x00);

    for(int p = 1; p < 100; p ++){
      //  int p = random(0, 25);
      //Serial.println(p);
      //BlinkM_setRGB( p, 0x00, 0x00, 0x00 );
      //delay(50);
      for(commandrepeatcounter = 0; commandrepeatcounter < commandrepeats; commandrepeatcounter++){
        ///BlinkM_setFadeSpeed( p, sweepfadespeed);
        ///BlinkM_fadeToRGB( p, (byte)r, (byte)g, (byte)b  );
        BlinkM_setRGB(p, (byte)r, (byte)g, (byte)b );
      }
      //delay(random(100));
      //BlinkM_setFadeSpeed( p, random(100));
      //BlinkM_fadeToRGB( p, 0x00, 0x00, 0x00);
      //delay (random(250));
      //delay(sweepdelay-i);
    }


    //delay(750);
  }
}









int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

