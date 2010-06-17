

float vl_high_row = 0;
float vl_target_row = 0;
//float vl_linger_row = 0;
//float vl_dissipation = 0.4;

int r, g, b = 0;
int bottomR = 10;
int bottomG = 0;
int bottomB = 255;
int topR = 255;
int topG = 0;
int topB = 10;


void vl_update() {
  
//  vl_high_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
  
  //determine row by lerping
  vl_target_row = map(voiceLevel, 0, 255, Y_COUNT-1, 0);
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
  r = map(voiceLevel, 0, 255, bottomR, topR);
  g = map(voiceLevel, 0, 255, bottomG, topG);
  b = map(voiceLevel, 0, 255, bottomB, topB);
  
  //set color vals
  for(int y=0; y<Y_COUNT; y++) { 
    for(int x=0; x<X_COUNT; x++) {
      if(y>=(int)vl_high_row) {
        setPixel(x, y, r, g, b);
      } else {
        setPixel(x, y, 0, 0, 0);
      } 
    }
  }
  
}
