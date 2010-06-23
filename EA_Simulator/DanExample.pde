
void de_update() {
  //update pixels
  int aw = width/xCount;
  int ah = height/yCount;
  for(int y=0; y<yCount; y++) {
    for(int x=0; x<xCount; x++) {
      float dis = dist(mouseX, mouseY, x*aw + w/2, y*ah + h/2);   
      color col = round(map(dis, width/2, 0, 0, 255));
      pixelVals[x][y] = constrain(col, 0, 255);
    }  
  }
}
