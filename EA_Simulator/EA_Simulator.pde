/**
*  Excellent Adventure Simulator
*  
*  A simulator to help develop animations without the need for an arduino.
*
*
*/

import peasy.org.apache.commons.math.*;
import peasy.*;
import peasy.org.apache.commons.math.geometry.*;
import processing.opengl.*;

PeasyCam cam;

int[][] addresses;
color[][] pixelVals;

int xCount = 6; 
int yCount = 10; 

float w, h;

int animationIndex = 0;

void setup() {
  size(1024, 768, OPENGL);
  hint(ENABLE_OPENGL_4X_SMOOTH);
  
  cam = new PeasyCam(this, 100);
  cam.setMinimumDistance(50);
  cam.setMaximumDistance(2000);  
  
  //fill addresses
  int thisAddress = 5;
  addresses = new int[xCount][yCount];  
  for(int x=0; x<xCount; x++) {
    for(int y=yCount-1; y>=0; y--) {
      addresses[x][y] = thisAddress;
      thisAddress++;
    }  
  }
  
  pixelVals = new int[xCount][yCount]; 
  
  float sizeFactor = 100;
  w = (3.5 * sizeFactor) / (xCount/3);
  h = (7 * sizeFactor) / yCount;   
}

void draw() {
  
  switch(animationIndex) {
    case(0):
      de_update();
      break;
  }
  
  background(0);
  translate(-xCount*w/2, -yCount*h/2);
  
  //draw left face
  pushMatrix();
  translate(xCount/3*w, 0);
  rotateY(-PI/2);
  for(int x=0; x<xCount/3; x++) {
    for(int y=0; y<yCount; y++) {
      stroke(50);
      fill( pixelVals[xCount/3-x][y] );
      rect(x*w, y*h, w, h);        
    }
  }
  popMatrix();
  
  //draw middle face
  pushMatrix();
  for(int x=xCount/3; x<xCount*2/3; x++) {
    for(int y=0; y<yCount; y++) {
      stroke(50);
      fill( pixelVals[x][y] );
      rect(x*w, y*h, w, h);
    }  
  }
  popMatrix();  
  
  //draw right face
  pushMatrix();
  translate(xCount*2/3*w, 0);
  rotateY(-PI/2);
  for(int x=0; x<xCount/3; x++) {
    for(int y=0; y<yCount; y++) {
      stroke(50);
      int xIndex = xCount*2/3+x;
      fill( pixelVals[xIndex][y] );
      rect(x*w, y*h, w, h);
    }  
  }
  popMatrix();  
}
