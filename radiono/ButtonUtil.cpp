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
  while (val != val2) { // DeBounce Button Press
      delay(10);
      val2 = val;
      val = analogRead(FN_PIN);
  }
  
  if (val>1000) return 0;
  
  tuningLocked = 1; // Holdoff Tuning until button is processed
  // 47K Pull-up, and 4.7K switch resistors,
  // Val should be approximately = (btnN×4700)÷(47000+4700)×1023

  DEBUG(P("%s %d: btn Val= %d"), __func__, __LINE__, val);

  if (val > 350) return 7;
  if (val > 300) return 6;
  if (val > 250) return 5;
  if (val > 200) return 4;
  if (val > 150) return 3;
  if (val >  50) return 2;
  return 1;
}


// -------------------------------------------------------------------------------
void deDounceBtnRelease() {
  int i = 2;
    
    while (i--) { // DeBounce Button Release, Check twice
      while (btnDown()) delay(20);
    }
    // The following allows the user to re-center the
    // Tuning POT during any Key Press-n-hold without changing Freq.
    readTuningPot();
    knobPositionPrevious = knobPosition;
    tuningLocked = 0; // Allow Tuning to Proceed
}


// -------------------------------------------------------------------------------
void decodeAux(int btn) {
    
    //debug("%s btn %d", __func__, btn);
    sprintf(c, P("Btn: %.2d"), btn);
    printLineCEL(STATUS_LINE, c);
    delay(100);
    deDounceBtnRelease(); // Wait for Release
}


// ###############################################################################
int checkForAltPress(int btn, int tbtn) {
    
    if (btn != FN_BTN     && tbtn == FN_BTN)     return ALT_PRESS_FN;
    if (btn != LT_CUR_BTN && tbtn == LT_CUR_BTN) return ALT_PRESS_LT_CUR;
    if (btn != RT_CUR_BTN && tbtn == RT_CUR_BTN) return ALT_PRESS_RT_CUR;
    if (btn != LT_BTN     && tbtn == LT_BTN)     return ALT_PRESS_LT;
    if (btn != UP_BTN     && tbtn == UP_BTN)     return ALT_PRESS_UP;
    if (btn != DN_BTN     && tbtn == DN_BTN)     return ALT_PRESS_DN;
    if (btn != RT_BTN     && tbtn == RT_BTN)     return ALT_PRESS_RT;
    
    return 0;
}


// -------------------------------------------------------------------------------
int getButtonPushMode(int btn) {
    int t1, t2, tbtn;
    int altPress;
  
    t1 = t2 = 0;

    // Time for first press
    tbtn = btnDown();
    while (t1 < 20 && btn == tbtn){
        tbtn = btnDown();
        altPress = checkForAltPress(btn, tbtn);
        if(altPress) return altPress;
        delay(50);
        t1++;
    }
    // Time between presses
    while (t2 < 10 && !tbtn){       
        tbtn = btnDown();
        altPress = checkForAltPress(btn, tbtn);
        if(altPress) return altPress;
        delay(50);
        t2++;
    }

    if (t1 > 10) return LONG_PRESS;
    if (t2 < 7) return DOUBLE_PRESS; 
    return MOMENTARY_PRESS; 
}


// End
