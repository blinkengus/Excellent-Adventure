/*
 ______ ______ __   __      __     __ ______ __  __ ______ __  __ ______  
/\  ___\\  == \\ "-.\ \    /\ \  _ \ \\  == \\ \/ / \  ___\\ \_\ \\  == \ 
\ \ \____\  __< \ \-.  \   \ \ \/ ".\ \\  __< \  _"-.\___  \\  __ \\  _-/ 
 \ \_____\\_____\\ \_\\"\   \ \ \__/".~\\ \_\ \\ \_\ \\_____\\_\ \_\\_\   
  \/_____//_____//_/ \/_/    \/_/   \/_//_/ /_//_/\/_//_____//_/\/_//_/   


                                                                                                     
    ---What?---
    Some handy functions that are not included in Arduino by default.
    
    ---Why?---
    For linear interpolation of PWM values (colors) in Excellent Adventure.
    
    ---Who?---
    Daniel Massey - pichiste@gmail.com
      
    ---When?---
    June 19, 2010                                                                                             
*/

float lerp(float start, float stop, float amt) {
   return start + (stop-start) * amt;
}
