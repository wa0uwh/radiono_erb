// EditIF.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "EditIF.h"
#include "debug.h"

boolean editIfMode = false;


// ###############################################################################
void editIf() {  // Set the IF Frequency
    static int vfoActivePrev = VFO_A;
    static boolean sbActivePrev;
    
    #ifdef USE_PARK_CURSOR
        cursorDigitPosition = 0;
    #endif // USE_PARK_CURSOR

    if (editIfMode) {  // Save IF Freq, Reload Previous VFO
        frequency += ritVal;
        isLSB ? iFreqLSB = frequency : iFreqUSB = frequency;
        isLSB = sbActivePrev;
        frequency = (vfoActivePrev == VFO_A) ? vfoA : vfoB;
    }
    else {  // Save Current VFO, Load IF Freq 
        vfoActive == VFO_A ? vfoA = frequency : vfoB = frequency;
        vfoActivePrev = vfoActive;
        sbActivePrev = isLSB;
        frequency = isLSB ? iFreqLSB : iFreqUSB;
    }
    editIfMode = !editIfMode;  // Toggle Edit IF Mode    
    #ifdef USE_PARK_CURSOR
        cursorDigitPosition = 0;
    #endif // USE_PARK_CURSOR 
    tune2500Mode = 0;
    ritOn = ritVal = 0;
}

// End
