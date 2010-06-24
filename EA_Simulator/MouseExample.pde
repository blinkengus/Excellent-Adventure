/**
*
* Example sketch. 
* The update function needs to go in the switch statement in the main loop()
*
*/


void me_update() {
  //update pixels
  int aw = width/xCount;
  int ah = height/yCount;
  for(int y=0; y<yCount; y++) {
    for(int x=0; x<xCount; x++) {
      float dis = dist(mouseX, mouseY, x*aw + w/2, y*ah + h/2);   
      int val = round(map(dis, width/2, 0, 0, 255));
      setPixel(x, y, val, val, val);
    }  
  }
}
