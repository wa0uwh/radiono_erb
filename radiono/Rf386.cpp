//Rf386.cpp
// Support for RF386 Filter Selection

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
#include "debug.h"

#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug

void setRf386BandSignal(unsigned long freq){
  // This setup is similar to the Minima RF386 RF Power Amplifier
  // Except now Reset (i.e., Filter 0) is "No Filter"
  // See: http://www.hfsignals.org/index.php/RF386

  // Bitbang Clock Pulses to Change PA Band Filter
  int band;
  static int prevBand = -1;
  static unsigned long prevFreq = 0;

  if (freq == prevFreq) return;
  prevFreq = freq;
  
  
  // Note: This is reverse of the Original Proposed band switching order, see above link 
  band = 0;                              // Assumes Null Filter
  if      (freq <   4.0 * MHz) band = 1; //   3.5 MHz
  else if (freq <  10.2 * MHz) band = 2; //  7-10 MHz
  else if (freq <  18.2 * MHz) band = 3; // 14-18 MHz
  else if (freq <  30.0 * MHz) band = 4; // 21-28 MHz
  else if (freq <  60.0 * MHz) band = 5; // 50-52 MHz
  else if (freq < 100.0 * MHz) band = 6; // 70-71 MHz
  else if (freq < 150.0 * MHz) band = 7; // 144-148 MHz
  else band = 7;

  //debug("Band Index = %d", band);
  
  if (band == prevBand) return;
  prevBand = band;
  
  DEBUG(P("BandIndex = %d"), band);
  pinMode(PA_BAND_CLK, OUTPUT);
  
  digitalWrite(PA_BAND_CLK, 1);  // Output Reset Pulse for PA Band Filter
  delay(10);
  digitalWrite(PA_BAND_CLK, 0);

  while (band > 0) { // Output Selector Pulses to Change PA Band Filter
     delay(2);
     digitalWrite(PA_BAND_CLK, 1);
     band--;
     digitalWrite(PA_BAND_CLK, 0);
  }
}

//End
