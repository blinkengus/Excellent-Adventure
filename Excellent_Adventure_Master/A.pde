// Effect A
// Row Climber #1  --  Daniel Massey 06/11/2010
//
// Effect incrementally lights whole rows with white, varying the
// brilliance to match the voice level.

int d_row = 2;
int d_waitTime = 7;
int d_counter = 0;

void d_update() {
  
  //set color vals
  for(int y=0; y<Y_COUNT; y++) { 
    for(int x=0; x<X_COUNT; x++) {
      if(y==d_row) {
        setPixel(x, y, voiceLevel, voiceLevel, voiceLevel);
      } else {
        setPixel(x, y, 0, 0, 0);
      }    
    }
  }
  
  //update row position
  if(d_counter>=d_waitTime) {
    if(d_row<=0) {
      d_row = 2;
    } else {
      d_row--;  
    }
    d_counter = 0;
  } else {
    d_counter++;  
  }
}
