/*

Some handy functions that aren't included in Arduino.

*/

float lerp(float start, float stop, float amt) {
   return start + (stop-start) * amt;
}
