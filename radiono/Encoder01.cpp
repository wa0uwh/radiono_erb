// Encoder01.cpp


#include <Arduino.h>
#include "A1Main.h"

#ifdef USE_ENCODER01

#include "Encoder01.h"
#include "ButtonUtil.h"
#include "debug.h"

static char knob;


// ###############################################################################
// ###############################################################################
void initEncoder() {
     // NoOp
}


// ###############################################################################
int getEncoderDir() {
    char tmp = knob;
      
    if (tmp>0) { knob--; return +1;}
    if (tmp<0) { knob++; return -1;}
    return 0;
}

// ###############################################################################
void readEncoder(int btn) {
    static unsigned long startTime = 0;
    unsigned long tigermillis;
    
    tigermillis = millis();
    
    if (tigermillis-startTime <= ISR_DEBOUNCE_TIMEOUT) return;
    startTime=tigermillis;
 
    knob += analogRead(ANALOG_TUNING) > 50 ? -1 : +1;
    
    //debug("%s/%d: Knob= %d", __func__, __LINE__, knob);
    return;
}

#endif // USE_ENCODER01
// End
