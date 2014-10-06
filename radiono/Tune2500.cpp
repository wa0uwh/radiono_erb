// Tune2500.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "Tune2500.h"
#include "debug.h"

int tune2500Mode = 0;

// ###############################################################################
void decodeTune2500Mode() {
    
    if (editIfMode) return; // Do Nothing if in Edit-IF-Mode   
    if (ritOn) return; // Do Nothing if in RIT Mode
    
    cursorDigitPosition = 3; // Set default Tuning Digit
    tune2500Mode = !tune2500Mode;
    dialCursorMode = false;
    if (tune2500Mode) frequency = (frequency / 2500) * 2500;
}

// End
