// Encoder01.cpp


#include <Arduino.h>
#include "A1Main.h"

#ifdef USE_ENCODER01

#include "Encoder01.h"
#include "ButtonUtil.h"
#include "debug.h"

static int knob;


// ###############################################################################
// ###############################################################################
void initEncoder() {
     // NoOp
}


// ###############################################################################
int getEncoderDir() {
    int dir = 0;
    
    if (knob) {
        dir = knob > 0 ? +1 : knob < 1 ? -1 : 0;;
        debug("%s/%d: btn= %d, Dir= %d", __func__, __LINE__, knob, dir);
        knob = 0;
    }  
    return dir;
}

// ###############################################################################
int readEncoder(int btn) {
    
        knob = analogRead(ANALOG_TUNING) > 50 ? -1 : +1;
        debug("%s/%d: Knob= %d", __func__, __LINE__, knob);
}

#endif // USE_ENCODER01
// End
