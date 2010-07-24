// Graphing sketch

// This program takes ASCII-encoded strings
// from the serial port at 9600 baud and graphs them. It expects values in the
// range 0 to 1023, followed by a newline, or newline and carriage return

// Created 20 Apr 2005
// Updated 18 Jan 2008
// by Tom Igoe

import processing.serial.*;

Serial myPort;        // The serial port
int xPos = 1;         // horizontal position of the graph
float y0 = 0;
int PORT  = 0;
int zeros = 0;

int FRAMESIZE = 8;
int HIGHS_DURATION = 10;
float HIGHS_ACCELERATION = -1;
float WIDTH = 64;
float HIGHS_HEIGHT = 3;
float[] highs = new float[FRAMESIZE];
float[] highsVelocity = new float[FRAMESIZE];
int[] highsCounter = new int[FRAMESIZE];
float XOFFSET;
float YOFFSET = 24;
float height0;


void setup()
{
    // set the window size:
    size((int)(FRAMESIZE * WIDTH), 512);        

    XOFFSET = (width - (FRAMESIZE * WIDTH)) / 2;
    height0 = (height-2*YOFFSET);
    for (int i = 0; i < FRAMESIZE; i++){
        highs[i] = 0;
        highsCounter[i] = 0;
        highsVelocity[i] = 0;
    }

    // List all the available serial ports
    println(Serial.list());
    // I know that the first port in the serial list on my mac
    // is always my  Arduino, so I open Serial.list()[0].
    // Open whatever port is the one you're using.
    myPort = new Serial(this, Serial.list()[PORT], 9600);
    // don't generate a serialEvent() unless you get a newline character:
    //myPort.bufferUntil('\n');
    // set inital background:
    background(0);

    char b = 164;    
    int ubyte = 0x0 | b;
    
    
    println (b + " " + ubyte);


}
void draw () {
    char[] buffer = new char[FRAMESIZE];

    if (myPort.available() >= FRAMESIZE)
    {
        //for (int i = 0; i < FRAMESIZE; i++)
        //{
        //    
        //}
        background(0);
        //println(buffer);
        for (int i = 0; i < FRAMESIZE; i++)
        {
            
            //buffer[i]);
            int ubyte = myPort.readChar();
            float y = map((float)ubyte, 0, 255, 
                0, height0);
            noStroke();
            
            if (y > highs[i])
            {
                highs[i] = y;
                highsCounter[i] = HIGHS_DURATION;
                highsVelocity[i] = 0;
            } else {
                if (highsCounter[i] < 0)
                {
                    highsVelocity[i] += HIGHS_ACCELERATION;
                    highs[i] += highsVelocity[i];
                    highs[i] = max(0, highs[i]);
                } else {
                    
                    highsCounter[i]--;
                }
            }

            fill(255, 255 * (y / height0), 0);
            rect( XOFFSET + i * WIDTH, YOFFSET+height0-y, WIDTH, y);
            fill(192, 192, 192);
            rect( XOFFSET + i * WIDTH, YOFFSET+height0 - highs[i], 
                WIDTH, -HIGHS_HEIGHT);
        }
    }
    // everything happens in the serialEvent()
}

void serialEvent (Serial myPort) 
{
/*
        String inString = myPort.readStringUntil('\n');

    if (inString != null) {
        // trim off any whitespace:
        inString = trim(inString);
        // convert to an int and map to the screen height:
        float data = float(inString); 
        float y = map(data, -32768, 32767, 0, height );

        // draw the line:
        stroke(127,34,255);
        line(max(xPos-1,0), y0, xPos, y);
        y0 = y;
        // at the edge of the screen, go back to the beginning:
        if (xPos >= width) {
            xPos = 0;
            background(0); 
        } 
        else {
            // increment the horizontal position:
            xPos++;
        }
    }
*/
}


