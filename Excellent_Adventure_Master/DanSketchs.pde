/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   


                                                                                                     
    ---What?---
    Visualization modes for Excellent Adventure booths.
    
    ---Why?---
    See Excellent Adventure Master
      
    ---Who?---
    Daniel Massey - pichiste@gmail.com
  
    ---When?---
    June 16, 2010                                                                                             
*/

//////////////////////////////////////////////////////////////////
// VERTICAL LEVEL

float vl_high_row = 0;
float vl_target_row = 0;
//float vl_linger_row = 0;
//float vl_dissipation = 0.4;

int vl_r, vl_g, vl_b = 0;
int vl_bottomR = 10;
int vl_bottomG = 0;
int vl_bottomB = 255;
int vl_topR = 255;
int vl_topG = 0;
int vl_topB = 10;


void vl_update() {
  
//  vl_high_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
  
  //determine row by lerping
  vl_target_row = map(voiceLevel, 0, 120, Y_COUNT-1, 0) + 1;
  vl_high_row = round(lerp(vl_high_row, vl_target_row, 0.7));
  
//  //determine high level
//  vl_high_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
//  
//  //smooth the drop
//  if(vl_high_row < vl_linger_row) {
//    vl_linger_row = vl_high_row;  
//  } else {
//    vl_high_row = vl_linger_row;  
//  }
//  vl_linger_row += vl_dissipation;
  
  //determine color
  vl_r = map(voiceLevel, 0, 255, vl_bottomR, vl_topR);
  vl_g = map(voiceLevel, 0, 255, vl_bottomG, vl_topG);
  vl_b = map(voiceLevel, 0, 255, vl_bottomB, vl_topB);
  
  //set color vals
  for(int y=0; y<Y_COUNT; y++) { 
    for(int x=0; x<X_COUNT; x++) {
      if(y>=(int)vl_high_row) {
        setPixel(x, y, vl_r, vl_g, vl_b);
      } else {
        setPixel(x, y, 0, 0, 0);
      } 
    }
  }
  
}

//////////////////////////////////////////////////////////////////
// TWINKLES

int cl_r, cl_g, cl_b = 0;
int cl_r1 = 0; 
int cl_g1 = 0;
int cl_b1 = 0;
int cl_r2 = 25;
int cl_g2 = 0;
int cl_b2 = 255;
float cl_angle = 0;
float cl_angle_inc;
float cl_angle_inc_min = PI/100;
float cl_angle_inc_max = PI/7;
float cl_pos = 0;


void cl_update() {
 
  float brightness = voiceLevel/255.0;
  
//  cl_angle_inc = voiceLevel*0.004 + cl_angle_inc_min;  //map(voiceLevel, 0, 255, cl_angle_inc_min, cl_angle_inc_max);
//  cl_pos = (sin(cl_angle)+1) / 2;     
//  cl_angle += cl_angle_inc;
  
//  cl_r = lerp(cl_r1, cl_r2, cl_pos);
//  cl_g = lerp(cl_g1, cl_g2, cl_pos);
//  cl_b = lerp(cl_b1, cl_b2, cl_pos); 
  
  //set color vals
  for(int y=0; y<Y_COUNT; y++) { 
    for(int x=0; x<X_COUNT; x++) {
      float localBrightness = brightness * random(1000)/1000.0;
      int r = random(255) * localBrightness;
      int g = random(255) * localBrightness;
      int b = random(255) * localBrightness;
      setPixel(x, y, r, g, b);
//      setPixel(x, y, cl_r, cl_g, cl_b);
    }
  }
  delay(50);
}

//////////////////////////////////////////////////////////////////
// VERTICAL SCANNER

int vs_row = 0;
int vs_interval;
int vs_min_interval = 2;
int vs_max_interval = 20;
int vs_counter = 0;
int vs_r = 255;
int vs_g = 255; 
int vs_b = 255;
int vs_rt = 255;
int vs_gt = 255;
int vs_bt = 255;
int vs_color_interval = 50;
int vs_color_counter = 0;

void vs_update()
{
  vs_interval = map(voiceLevel, 0, 255, vs_max_interval, vs_min_interval);
  
  if(vs_counter>=vs_interval) {
    if(vs_row>Y_COUNT-1) {
      vs_row = 0;    
    } else {
      vs_row++;  
    }
    vs_counter = 0;
  } else {
    vs_counter++;  
  }
  
  if(vs_color_counter<vs_color_interval) {
    vs_color_counter++;    
  } else {
    vs_rt = random(255);
    vs_gt = random(255);
    vs_bt = random(255);
    Serial.println(vs_r);    
    vs_color_counter = 0; 
  }
  
  //determine color
  vs_r = lerp(vs_r, vs_rt, 0.2);
  vs_g = lerp(vs_g, vs_gt, 0.2);
  vs_b = lerp(vs_b, vs_bt, 0.2);  
  
  for(int y=0; y<Y_COUNT; y++) {
    for(int x=0; x<X_COUNT; x++) {
      if(y==vs_row) {
        setPixel(x, y, vs_r, vs_g, vs_b);     
      } else {
        setPixel(x, y, 0x00, 0x00, 0x00);  
      }
    }  
  }
}

