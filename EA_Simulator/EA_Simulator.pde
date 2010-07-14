/**
 *  Excellent Adventure Simulator
 *  
 *  A simulator to help develop animations without the need for an arduino.
 */

import processing.opengl.*;
import javax.media.opengl.*;

import ddf.minim.*;
import ddf.minim.analysis.*;


int[][]   addresses;
int[][][] pixelVals;

static final int XCOUNT = 3;
static final int YCOUNT = 12;

static final int PANEL_XCOUNT = 1;
static final int PANEL_YCOUNT = 10;

static final int PANEL_TOP_XCOUNT = 3;
static final int PANEL_TOP_YCOUNT = 2;

float panel_spacing = 3.0;

int animationIndex = 0;

AudioInput lineIn;
FFT        lineInFFT;


void setup()
{
    size(500, 800, OPENGL);

    //cam = new PeasyCam(this, 0,0,0, 10);
    //cam.setMinimumDistance(5);
    //cam.setMaximumDistance(20);
    
    //lineIn = minim.getLineIn(Minim.MONO, 8);

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
}

void draw()
{
    switch(animationIndex) {
        case(0):
        me_update();
        break;
    }

    background(0);
    drawBooth();
}

void setPixel(int x, int y, int r, int g, int b)
{
    pixelVals[x][y][0] = r;
    pixelVals[x][y][1] = g;
    pixelVals[x][y][2] = b;
}

void drawBooth()
{
    perspective(PI / 2.0, (float)width / (float)height, 0.01, 200);
    camera(0,0,100, 0,0,0, 0,1,0);

    //    GL gl = ((PGraphicsOpenGL)g).beginGL();
    //    
    //    gl.glDisable(GL.GL_DEPTH_TEST);
    //    gl.glEnable(GL.GL_BLEND);
    //    gl.glBlendFunc(GL.GL_SRC_COLOR, GL.GL_ONE);

    scale(15,15,15);
    rotateY(map(mouseX, 0, width, -PI, PI));
    rotateX(map(mouseY, 0, height, PI, -PI));

    pushMatrix();
    translate((panel_spacing + 1) / -2.0, 0, 0);
    rotateY(HALF_PI);
    drawBoothPanel(0);
    popMatrix();

    pushMatrix();
    translate(0, 0, (panel_spacing + 1) / -2.0);
    drawBoothPanel(1);
    popMatrix();

    pushMatrix();
    translate((panel_spacing + 1) / 2.0, 0, 0);
    rotateY(-HALF_PI);
    drawBoothPanel(2);
    popMatrix();
    
    pushMatrix();
    translate(0, PANEL_YCOUNT / -2.0, 0);
    rotateX(HALF_PI);
    drawBoothTopPanel();
    popMatrix();

    //    ((PGraphicsOpenGL)g).endGL();
}
void drawBoothPanel(int panel_num)
{
    pushMatrix();
    translate(panel_spacing * (PANEL_XCOUNT - 1) / -2.0, (PANEL_YCOUNT - 1) / -2.0);
    for(int y = 0; y < PANEL_YCOUNT; y++) {
        for(int x = 0; x < PANEL_XCOUNT; x++) {
            pushMatrix();
            translate(x * panel_spacing, y, 0);
            drawBoothLED(panel_num * PANEL_XCOUNT + x, y, 1.0 / 3.0);
            popMatrix();
        }
    }
    popMatrix();
}
void drawBoothTopPanel()
{
    pushMatrix();
    translate((PANEL_TOP_XCOUNT - 1) / -2.0, (PANEL_TOP_YCOUNT - 1) / -2.0);
    for(int y = 0; y < PANEL_TOP_YCOUNT; y++) {
        for(int x = 0; x < PANEL_TOP_XCOUNT; x++) {
            pushMatrix();
            translate(x, y, 0);
            drawBoothLED(x, PANEL_YCOUNT + y, 1.0 / 3.0);
            popMatrix();
        }
    }
    popMatrix();
}
void drawBoothLED(int x, int y, float sc)
{
    float d = sc / 1.5;
    fill(pixelVals[x][y][0], pixelVals[x][y][1], pixelVals[x][y][2]);
    noStroke();
    ellipseMode(CENTER);
    for(int ix = -1; ix <= 1; ix++) {
        for(int iy = -1; iy <= 1; iy++) {
            ellipse(ix * sc, iy * sc, d, d);
        }
    }
}

