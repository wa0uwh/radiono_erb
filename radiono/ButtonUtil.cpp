// ButtonUtil.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "ButtonUtil.h"
#include "PotKnob.h"
#include "debug.h"


// ###############################################################################
int btnDown(){
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
  int val = -1, val2 = -2;
  
  val = analogRead(FN_PIN);
  while (abs(val - val2) > 3) { // DeBounce Button Press
      // delay(10);
      val2 = val;
      val = analogRead(FN_PIN);
  }
  
  if (val>1000) return 0;
  
  tuningLocked = 1; // Holdoff Tuning until button is processed
  // 47K Pull-up, and 4.7K switch resistors,
  // Val should be approximately = 1024*btnN*4700/(47000+(btnN*4700))
  // N = 0 to Number of button - 1

  DEBUG(P("%s/%d: btn Val= %d"), __func__, __LINE__, val);
  
 /* // Old Code
  if (val > 400) return 8; // Btn8 Used by the Rotary Encoder
  if (val > 350) return 7;
  if (val > 300) return 6;
  if (val > 250) return 5;
  if (val > 200) return 4;
  if (val > 150) return 3;
  if (val >  50) return 2;
  return 1;
 */ 

  // 1024L*b*4700L/(47000L+(b*4700L))   >>>  1024*b/(10+b);
  
  for(int b = MAX_BUTTONS - 1; b >= 0; b--) {
      if (val + 15 > 1024*b/(10+b)) return b+1;
  }
  return 1;
}


// -------------------------------------------------------------------------------
void deDounceBtnRelease() {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
  int i = 2;
    
    while (i--) { // DeBounce Button Release, Check twice
      while (btnDown()) {
          DEBUG(P("%s/%d:"), __func__, __LINE__);
          delay(20);
      }
    }  
    #ifdef USE_POT_KNOB
      // The following allows the user to re-center the
      // Tuning POT during any Key Press-n-hold without changing Freq.
      readPot();
      knobPositionPrevious = knobPosition;
    #endif // USE_POT_KNOB
    tuningLocked = 0; // Allow Tuning to Proceed
}


// -------------------------------------------------------------------------------
void decodeAux(int btn) { 
    
    debug("%s/%d: btn %d", __func__, __LINE__, btn);
    //sprintf(c, P("Btn: %.2d"), btn);
    //printLineCEL(STATUS_LINE, c);
    //delay(100);
    deDounceBtnRelease(); // Wait for Release
}


// ###############################################################################
int checkForAltPress(int btn, int altbtn) {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

    DEBUG(P("%s/%d:"), __func__, __LINE__);
   
    // Find the First Button that Matches AltBtn, and that is not Btn, else retrun 0
    for (int b = 1; b < btn; b++) {
        if (b == altbtn) return LONG_PRESS + b;
    }
    return 0;
}


// -------------------------------------------------------------------------------
int getButtonPushMode(int btn) {
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
    int t1, t2, tbtn;
    int altPress;

    DEBUG(P("%s/%d:"), __func__, __LINE__);
  
    t1 = t2 = 0;

    // Time for first press
    tbtn = btnDown();
    while (t1 < 20 && btn == tbtn){
        tbtn = btnDown();
        altPress = checkForAltPress(btn, tbtn);
        if (altPress) return altPress;
        delay(50);
        t1++;
    }
    // Time between presses
    while (t2 < 10 && !tbtn){       
        tbtn = btnDown();
        altPress = checkForAltPress(btn, tbtn);
        if (altPress) return altPress;
        delay(50);
        t2++;
    }

    if (t1 > 10) return LONG_PRESS;
    if (t2 < 7) return DOUBLE_PRESS; 
    return MOMENTARY_PRESS; 
}


// End
