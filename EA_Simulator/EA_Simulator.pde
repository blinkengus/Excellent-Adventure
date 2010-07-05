/**
 *  Excellent Adventure Simulator
 *  
 *  A simulator to help develop animations without the need for an arduino.
 */

import processing.opengl.*;
import javax.media.opengl.*;

import peasy.org.apache.commons.math.*;
import peasy.org.apache.commons.math.geometry.*;
import peasy.*;


PeasyCam cam;

int[][]   addresses;
int[][][] pixelVals;

static final int XCOUNT = 6;
static final int YCOUNT = 10;

static final int PANEL_XCOUNT = 6;
static final int PANEL_YCOUNT = 10;

int animationIndex = 0;


void setup()
{
    size(500, 800, OPENGL);

    cam = new PeasyCam(this, 0,0,0, 10);
    cam.setMinimumDistance(5);
    cam.setMaximumDistance(20);

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
    switch(animationIndex){
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
    GL gl = ((PGraphicsOpenGL)g).beginGL();
    
    gl.glDisable(GL.GL_DEPTH_TEST);
    gl.glEnable(GL.GL_BLEND);
    gl.glBlendFunc(GL.GL_SRC_COLOR, GL.GL_ONE);

    drawBoothPanel(0);
    
    ((PGraphicsOpenGL)g).endGL();
}
void drawBoothPanel(int panel_num)
{
    translate(-XCOUNT / 2.0, -YCOUNT / 2.0);
    for(int y = 0; y < YCOUNT; y++){
        for(int x = panel_num * XCOUNT, nx = panel_num * XCOUNT + XCOUNT; x < nx; x++){
            pushMatrix();
            translate(x, y, 0);
            drawBoothLED(x, y, 1);
            popMatrix();
        }
    }
}
void drawBoothLED(int x, int y, float sc)
{
    float d = sc / 3.0;
    fill(pixelVals[x][y][0], pixelVals[x][y][1], pixelVals[x][y][2]);
    noStroke();
    ellipseMode(CENTER);
    for(int ix = -1; ix <= 1; ix++){
        for(int iy = -1; iy <= 1; iy++){
            ellipse(ix * sc, iy * sc, d, d);
        }
    }
}
