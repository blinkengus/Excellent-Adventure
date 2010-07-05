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

float WIDTH = 64;

void setup () {
    // set the window size:
    size(1024, 512);        

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
}
void draw () {
    byte[] buffer = new byte[FRAMESIZE];

    if (myPort.available() >= 5)
    {
        myPort.readBytes(buffer);
        background(0);
        //println(buffer);
        for (int i = 0; i < FRAMESIZE; i++)
        {
            
            float y = map((float)buffer[i], 0, 255, 
                0, height);
            stroke(127,34,255);
            fill(204, 102, 0);
            
            rect( i * WIDTH, height, WIDTH, - y);
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


