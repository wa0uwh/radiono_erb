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
        //debug("%s btn %d", __func__, knob);    
        //debug("%s dir %d", __func__, dir);
        knob = 0;
    }  
    return dir;
}

// ###############################################################################
int getEncoderKnob(int btn) {
    
    if(btnDown() == ENC_KNOB) {
        knob = analogRead(ANALOG_TUNING);
        //debug("%s btn %d", __func__, btn);    
        //debug("%s val %d", __func__, knob);
        deDounceBtnRelease();
    }
}

// End
