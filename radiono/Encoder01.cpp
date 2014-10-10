// Encoder01.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "Encoder01.h"
#include "ButtonUtil.h"
#include "debug.h"


static int knob;


// ###############################################################################
// ###############################################################################
void initEncoder() {
     
}


// ###############################################################################
int getEncoderDir() {
    int dir = 0;
    
    if (knob) {
        dir = knob > 50 ? -1 : +1;
        //debug("%s/%d: btn= %d, Dir= %d", __func__, __LINE__, knob, dir);
        knob = 0;
    }  
    return dir;
}

// ###############################################################################
int readEncoder(int btn) {
    
        knob = analogRead(ANALOG_TUNING);
        //debug("%s/%d: Knob= %d", __func__, __LINE__, knob);
}

// End
