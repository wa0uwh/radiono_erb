// PotKnob.cpp

#include <Arduino.h>
#include "A1Main.h"
#include "PotKnob.h"
#include "Menus.h"
#include "MorseCode.h"
#include "debug.h"


// ###############################################################################
void readPot(){
    
    knobPosition = analogRead(ANALOG_TUNING);
}


// ###############################################################################
int getPotDir() {
#define AUTOTIMER_RATE_MS (200)
  static unsigned long AutoTimer = 0;
  
  // Compute tuningDaltaPosition from knobPosition
  knobPositionDelta = knobPosition - knobPositionPrevious;
  
  if (AutoTimer > millis()) return 0; // Auto Inc/Dec Timer
  
  if (knobPosition < DEAD_ZONE * 2 && AutoTimer < millis()) { // We must be at the Low end of the Tuning POT
      knobPositionDelta = -DEAD_ZONE;
      AutoTimer = millis() + AUTOTIMER_RATE_MS;
      if (knobPosition > DEAD_ZONE ) AutoTimer += AUTOTIMER_RATE_MS*3/2; // At very end of Tuning POT
  }
  if (knobPosition > 1023 - DEAD_ZONE * 2 && AutoTimer < millis()) { // We must be at the High end of the Tuning POT
      knobPositionDelta = +DEAD_ZONE;
      AutoTimer = millis() + AUTOTIMER_RATE_MS;
      if (knobPosition  < 1023 - DEAD_ZONE / 8) AutoTimer += AUTOTIMER_RATE_MS*3/2; // At very end of Tuning POT
  }
  
  // Check to see if Digit Change Action is Required, Otherwise Do Nothing via RETURN 
  if (abs(knobPositionDelta) < DEAD_ZONE) return 0;
  
  knobPositionPrevious = knobPosition;
  
  return knobPositionDelta < 0 ? -1 : knobPositionDelta > 0 ? +1 : 0;
    
}
