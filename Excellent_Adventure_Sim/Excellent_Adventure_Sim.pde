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

int[][] addresses;
int[][][] pixelVals;

float rotationX;
float rotationY;
float rotationX0;
float rotationY0;
float mouseX0;
float mouseY0;


static final char TRUE = 1;
static final char FALSE = 0;

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


static final int ANIMATION_MOUSE  =   0;
static final int ANIMATION_SERIAL =  1;

static final int I2C_FIRST_ADDRESS = 10;

float time0;
char timeReset = 0;
char resynch = TRUE;

int animationIndex = ANIMATION_SERIAL;

AudioInput lineIn;
FFT lineInFFT;

int pixelCounter = -1;

void setup()
{
    size(500, 800, OPENGL);

    //cam = new PeasyCam(this, 0,0,0, 10);
    //cam.setMinimumDistance(5);
    //cam.setMaximumDistance(20);

    //lineIn = minim.getLineIn(Minim.MONO, 8);

    // List all the available serial ports
    println(Serial.list());
    // I know that the first port in the serial list on my mac
    // is always my  Arduino, so I open Serial.list()[0].
    // Open whatever port is the one you're using.
    port = new Serial(this, Serial.list()[0], 38400);


    //fill addresses
    int thisAddress = 5;
    addresses = new int[XCOUNT][YCOUNT];
    for(int x = 0; x < XCOUNT; x++) {
        for(int y = YCOUNT-1; y >= 0; y--) {
            addresses[x][y] = thisAddress;
            thisAddress++;
        }
    }

    pixelVals = new int[XCOUNT][YCOUNT][3];
    SetDisabledPixels();
    rotationX = width / 2;
    rotationY = height / 2;
}


void draw()
{
    if (resynch == TRUE)
    {
        SynchronizeSerialFrames();
    }
    switch(animationIndex) 
    {
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
    while (port.available() > 0)
    {
        int v = port.readChar();
        if (v == 255)
        {
            count255++;
            if (count255 >= 4)
            {
                resynch = FALSE;
                break;
            }
        } else {
            count255 = 0;
        }
    }
}


void ProcessSerialAnimation()
{
    //println(port.available());
    while (port.available() >= 4)
    {
        int who = port.readChar();
        int r = port.readChar();
        int g = port.readChar();
        int b = port.readChar();

        // Synchronization frame (all 255 values)
        if ( (who & r & g & b) == 0xFF)
        {
            pixelCounter = 0;
            continue;
        }

        // Benchmarking code
        if (who == 253)
        {
            time0 = millis();
            continue;
        }
        if (who == 254)
        {
            float dTime = millis() - time0;
            println("** time: " + dTime);
            timeReset = 0;
            continue;
        }

        // Our I2C addresses are offset by 10, however, our LED "addresses"
        // begin at zero.
        who -= I2C_FIRST_ADDRESS;
        int x = who % XCOUNT;
        int y = who / XCOUNT;

        if 
        (
            (x >= 0) 
        &&  (y >= 0) 
        &&  (x < XCOUNT) 
        &&  (y < PANEL_YCOUNT)

        )
        {
            setPixel(x, y, r, g, b);
            println(pixelCounter + ": (" + (who+10) + ") " + x + " " + y + " -> " + r + " " + g + " " + b);
        } else {
            println(pixelCounter + ": (" + (who+10) + ") " + x + " " + y + 
                " -> " + r + " " + g + " " + b + " - ERROR");
            resynch = TRUE;
            return;
        }
        pixelCounter++;

    }
}

void SetDisabledPixels()
{
    // A disabled pixel has a negative red value, and is skipped when drawing
    // to the panels.  The green value will used for a grey value in the box
    // if the value is >= 0, and transparent if < 0

    //   0 1 2    3 4 5    6 7 8
    // [ o . o ][ o . o ][ o . o ] 0
    // [ o . o ][ o . o ][ o . o ] 1
    // [ o . o ][ o . o ][ o . o ] 2
    // [ o . o ][ o . +----+ . o ] 3
    // [ o . o ][ o . |    | . o ] 4
    // [-------][-----|    |-----]  
    // [ o . +----+ . |    | . o ] 5
    // [ o . |    | . +----+ . o ] 6
    // [ o . +----+ . o ][ o . o ] 7
    // [ o . o ][ o . o ][ o . o ] 8
    // [ o . o ][ o . o ][ o . o ] 9 

    for (int y = 0; y < PANEL_YCOUNT; y++)
    {
        for (int x = 0; x < XCOUNT; x++)
        {
            pixelVals[x][y][0] =
                pixelVals[x][y][1] =
                pixelVals[x][y][2] = 0;
        }
    }

    // Middle column
    for (int y = 0; y < PANEL_YCOUNT; y++)
    {
        pixelVals[1][y][0] =
            pixelVals[4][y][0] =
            pixelVals[7][y][0] = -1;
        pixelVals[1][y][1] =
            pixelVals[4][y][1] =
            pixelVals[7][y][1] = -1; //transparent
    }

    // Battery
    for (int y = 5; y < 8; y++)
    {
        pixelVals[2][y][0] =
            pixelVals[3][y][0] = -1;
        pixelVals[2][y][1] =
            pixelVals[3][y][1] = 31;
    }
    // Phone
    for (int y = 3; y < 7; y++)
    {
        pixelVals[5][y][0] =
            pixelVals[6][y][0] = -1;
        pixelVals[5][y][1] =
            pixelVals[6][y][1] = 63;
    }
    pixelVals[0][PANEL_YCOUNT][0] =
        pixelVals[2][PANEL_YCOUNT][0] =
        pixelVals[0][PANEL_YCOUNT+2][0] =
        pixelVals[2][PANEL_YCOUNT+2][0] = -1;


}


void setPixel(int x, int y, int r, int g, int b)
{
    if (pixelVals[x][y][0] >= 0)
    {
        pixelVals[x][y][0] = r;
        pixelVals[x][y][1] = g;
        pixelVals[x][y][2] = b;
    }
}

void mousePressed ()
{
    mouseX0 = mouseX;
    mouseY0 = mouseY;
    rotationX0 = rotationX;
    rotationY0 = rotationY;
}

void mouseDragged ()
{
    rotationX = rotationX0 + mouseX - mouseX0;
    rotationY = rotationY0 + mouseY - mouseY0;
}


void drawBooth()
{
    perspective(PI / 2.0, (float)width / (float)height, 0.01, 200);
    camera(0,0,100, 0,0,0, 0,1,0);

    // GL gl = ((PGraphicsOpenGL)g).beginGL();
    //
    // gl.glDisable(GL.GL_DEPTH_TEST);
    // gl.glEnable(GL.GL_BLEND);
    // gl.glBlendFunc(GL.GL_SRC_COLOR, GL.GL_ONE);


    scale(15,15,15);
    rotateY(map(rotationX, 0, width, -PI, PI));
    rotateX(map(rotationY, 0, height, PI, -PI));


    // +---------+   z
    // |    1    |   ^
    // |         |   |
    // | 0     2 |   |
    // |         |   +----> x
    // |         |
    //

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

    // ((PGraphicsOpenGL)g).endGL();
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
    translate(  -(PIXEL_SPACING * (PANEL_TOP_XCOUNT - 1) / 2.0 + PANEL_MARGIN), 
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

    if ((pixelVals[x][y][0] < 0) && (pixelVals[x][y][1] >= 0))
    {
        fill(pixelVals[x][y][1], pixelVals[x][y][1], pixelVals[x][y][1]);
    } else {
        noFill();
    }

    stroke(31, 31, 31);
    rect(-0.5, -0.5, 1, 1);

    if (pixelVals[x][y][0] >= 0)
    {
        noStroke();
        //fill (255, 255, 255);
        fill(pixelVals[x][y][0], pixelVals[x][y][1], pixelVals[x][y][2]);
        ellipseMode(CENTER);
        for(int ix = -1; ix <= 1; ix++) {
            for(int iy = -1; iy <= 1; iy++) {
                ellipse(ix * sc, iy * sc, d, d);
            }
        }

    }
}



