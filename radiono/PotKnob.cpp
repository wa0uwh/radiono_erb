// PotKnob.cpp

/*
 * Copyright (C) 2014, by Author: Eldon R. Brown (ERB) - WA0UWH
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
