/**
* Excellent Adventure Simulator
*
* A simulator to help develop animations without the need for an arduino.
*/

import processing.opengl.*;
import javax.media.opengl.*;

import ddf.minim.*;
import ddf.minim.analysis.*;
import processing.serial.*;

Serial port;        // The serial port

int[] pixelVals;

float rotationX;
float rotationY;
float rotationX0;
float rotationY0;
float mouseX0;
float mouseY0;

static final int PANELS = 3;


static final int PANEL_XCOUNT = 3;
static final int PANEL_YCOUNT = 10;

static final int PANEL_TOP_XCOUNT = 3;
static final int PANEL_TOP_YCOUNT = 3;

static final int XCOUNT = PANELS * PANEL_XCOUNT;
static final int YCOUNT = PANEL_YCOUNT + PANEL_TOP_YCOUNT;

static final int BUFFER_SIZE = 3 * PANEL_XCOUNT * PANEL_YCOUNT;


static final float LED_SPACING = (1.0 / 3.0);

static final float PIXEL_SPACING = 1.1;
static final float PANEL_MARGIN  = 0.02;

static final float PANEL_XOFFSET = 
    PANEL_MARGIN + (PANEL_XCOUNT * PIXEL_SPACING) / 2.0;

static final float PANEL_YOFFSET = 
    PANEL_MARGIN + (PANEL_YCOUNT * PIXEL_SPACING) / 2.0;

static final int ANIMATION_MOUSE  = 0;
static final int ANIMATION_SERIAL = 1;

int animationIndex = ANIMATION_SERIAL;


void setup()
{
    size(500, 800, OPENGL);

    // List all the available serial ports
    println(Serial.list());

    // I know that the first port in the serial list on my mac
    // is always my  Arduino, so I open Serial.list()[0].
    // Open whatever port is the one you're using.
    port = new Serial(this, Serial.list()[0], 38400);

    pixelVals = new int[XCOUNT * YCOUNT];

    rotationX = width / 2;
    rotationY = height / 2;
    
    SynchronizeSerialFrames();
}


void draw()
{
    switch(animationIndex){
        case ANIMATION_MOUSE:
        default:
            //me_update();
            break;
        case ANIMATION_SERIAL:
            ProcessSerialAnimation();
            break;
    }

    background(0);
    drawBooth();
}


void SynchronizeSerialFrames()
{
    // Sync on a [255,255,255,255] frame.
    int count255 = 0;
    while (port.available() >= 1){
        int v = port.readChar();
        if(v == 255){
            count255++;
            if(count255 >= 4)
                break;
        }
        else{
            count255 = 0;
        }
    }
}


void ProcessSerialAnimation()
{
    //println(port.available());
    while(port.available() >= 4){
        int r = port.readChar();
        int g = port.readChar();
        int b = port.readChar();
        int addr = port.readChar();

        if(addr == 255)
            continue;

        int pixel_idx = addr - 10;//pixelAddresses[addr] - 10;
        
        if(pixel_idx >= 0 && pixel_idx < pixelVals.length)
            setPixelByIndex(pixel_idx, r, g, b);
    }
}


void setPixel(int x, int y, int r, int g, int b)
{
    setPixelByIndex(x + y * XCOUNT, r, g, b);
}
void setPixelByIndex(int index, int r, int g, int b)
{
    pixelVals[index] = color(r, g, b);
}


void mousePressed(){
    mouseX0 = mouseX;
    mouseY0 = mouseY;
    rotationX0 = rotationX;
    rotationY0 = rotationY;
}
void mouseDragged(){
    rotationX = rotationX0 + mouseX - mouseX0;
    rotationY = rotationY0 + mouseY - mouseY0;
}
void keyPressed(){
    for(int i = 0; i < pixelVals.length; i++)
        pixelVals[i] = 0;
    SynchronizeSerialFrames();
}


void drawBooth()
{
    perspective(PI / 2.0, (float)width / (float)height, 0.01, 200);
    camera(0,0,100, 0,0,0, 0,1,0);

    scale(15,15,15);
    rotateY(map(rotationX, 0, width, -PI, PI));
    rotateX(map(rotationY, 0, height, PI, -PI));

    // +---------+   z
    // |    1    |   ^
    // |         |   |
    // | 0     2 |   |
    // |         |   +----> x
    // |         |

    pushMatrix();
    //translate((PANEL_SPACING + 1) / -2.0, 0, 0);
    translate(-PANEL_XOFFSET, 0, 0);
    rotateY(HALF_PI);
    drawBoothPanel(0);
    popMatrix();

    pushMatrix();
    //translate(0, 0, (PANEL_SPACING + 1) / -2.0);
    translate(0, 0, -PANEL_XOFFSET);
    drawBoothPanel(1);
    popMatrix();

    pushMatrix();
    //translate((PANEL_SPACING + 1) / 2.0, 0, 0);
    translate(PANEL_XOFFSET, 0, 0);
    rotateY(-HALF_PI);
    drawBoothPanel(2);
    popMatrix();
    
    pushMatrix();
    //translate(0, PANEL_YCOUNT / 2.0, 0);
    translate(0, -PANEL_YOFFSET, 0);
    rotateX(HALF_PI);
    drawBoothTopPanel();
    popMatrix();
}
void drawBoothPanel(int panel_num)
{
    pushMatrix();
    translate(  -(PIXEL_SPACING * (PANEL_XCOUNT - 1) / 2.0 + PANEL_MARGIN), 
                -(PIXEL_SPACING * (PANEL_YCOUNT - 1) / 2.0 + PANEL_MARGIN));
    //translate(-PANEL_XOFFSET, -PANEL_XOFFSET);
    for(int y = 0; y < PANEL_YCOUNT; y++) {
        for(int x = 0; x < PANEL_XCOUNT; x++) {
            pushMatrix();
            translate(x * PIXEL_SPACING, y * PIXEL_SPACING, 0);
            drawBoothLED(panel_num * PANEL_XCOUNT + x, y, 1.0 / 3.0);
            popMatrix();
        }
    }
    popMatrix();
}
void drawBoothTopPanel()
{
    pushMatrix();
    translate(-(PIXEL_SPACING * (PANEL_TOP_XCOUNT - 1) / 2.0 + PANEL_MARGIN), 
              -(PIXEL_SPACING * (PANEL_TOP_YCOUNT - 1) / 2.0 + PANEL_MARGIN));
    for(int y = 0; y < PANEL_TOP_YCOUNT; y++) {
        for(int x = 0; x < PANEL_TOP_XCOUNT; x++) {
            pushMatrix();
            translate(x * PIXEL_SPACING, y * PIXEL_SPACING, 0);
            drawBoothLED(x, PANEL_YCOUNT + y, LED_SPACING);
            popMatrix();
        }
    }
    popMatrix();
}
void drawBoothLED(int x, int y, float sc)
{
    float d = sc / 1.5;

    int index = x + y * XCOUNT;

    noFill();
    stroke(31, 31, 31);
    rect(-0.5, -0.5, 1, 1);

    noStroke();
    fill(pixelVals[index]);
    ellipseMode(CENTER);
    for(int ix = -1; ix <= 1; ix++) {
        for(int iy = -1; iy <= 1; iy++) {
            ellipse(ix * sc, iy * sc, d, d);
        }
    }
}
