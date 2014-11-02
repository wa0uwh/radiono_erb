// Scanner.cpp

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
#include "Scanner.h"
#include "Tune2500.h"
#include "debug.h"
  
#ifdef USE_AUTOSCANNER

char autoScanMode = 0;
int autoScanRate = 0;

char vfoScanStash = 0;
char vfoHiLimit = 0;
char vfoLoLimit = 0;


// ####################################################################
void autoScanStop(int vfo) {
    
    if (vfo == SCAN_STOP_ON_CURRENT_FREQ) { // Stop on Scanned Freq
        vfos[vfoScanStash] = vfos[vfoActive];
        vfoActive = vfoScanStash;
    }
    else if (vfo == SCAN_STOP_USE_PREV_VFO) vfoActive = vfoScanStash; // Use Previous VFO     
    else vfoActive = vfo;  // Use Specified VFO
    autoScanMode = 0;
    refreshDisplay++;
    return;
}

// ####################################################################
int autoScanInit(int scanMode, int dir) {
 
      if(!cursorDigitPosition) {autoScanStop(vfoActive); return false;}
      
      //scanInitialDir = 0;
      autoScanRate = 0;
                
      knobMode = KNOB_DIGIT_MODE;
        
      switch (scanMode) {
          case SCAN_STOP:
              autoScanStop(SCAN_STOP_USE_PREV_VFO);
              return false;
              break;
              
          case SCAN_SIMPLE:
              vfoScanStash = vfoActive;
              vfoActive = VFO_S;
              if (!autoScanRate) autoScanRate = dir;
              autoScanRate = abs(autoScanRate) * dir;
              autoScanMode = scanMode;
              return true;
              break;
              
          case SCAN_UP_AB:
          case SCAN_DN_AB:
          case SCAN_BETWEEN_AB: // Scan loop between VFO_A and VFO_B
              if (vfos[VFO_A] == vfos[VFO_B]) { autoScanStop(SCAN_STOP_USE_PREV_VFO); return false; }
              
              vfoScanStash = vfoActive;
              vfos[VFO_S] = vfos[vfoActive];
              vfoActive = VFO_S;
              
              if (vfos[VFO_A] < vfos[VFO_B]) {
                  vfoLoLimit = VFO_A; vfoHiLimit = VFO_B;
              }
              else {
                  vfoLoLimit = VFO_B; vfoHiLimit = VFO_A;
              }
              
              if (!autoScanRate) autoScanRate = dir;
              autoScanRate = abs(autoScanRate) * dir;
              autoScanMode = scanMode;
              return true;
              break;
      }    
}


// ####################################################################
int getAutoScanDir() { // AutoScan Frequency Mode
    static unsigned long autoScanTimer = 0;
    static unsigned long autoScanTimeout = 0;
 
      if(!autoScanMode) return tuningDir;
  
      if(!cursorDigitPosition) { autoScanStop(SCAN_STOP_USE_PREV_VFO); return 0; }

      if (tuningDir) {
          autoScanRate = constrain (autoScanRate + tuningDir, -8, +8);
          autoScanTimer = 0;
      }
      if (autoScanTimer > millis()) return 0;
      
      int rateInc = 200;
      #ifdef USE_TUNE2500_MODE
          if (tune2500Mode) rateInc *= 3;
      #endif // USE_TUNE2500_MODE
      autoScanTimer = millis() + 100 + rateInc * (8 - abs(autoScanRate));
      refreshDisplay++;
      
      switch (autoScanMode) {
          case SCAN_BETWEEN_AB:
              if (vfos[VFO_S] >= vfos[vfoHiLimit]) autoScanRate = -abs(autoScanRate);
              if (vfos[VFO_S] <= vfos[vfoLoLimit]) autoScanRate = +abs(autoScanRate);
              break;
          case SCAN_UP_AB:
          case SCAN_DN_AB:
              if (autoScanRate > 0 && vfos[VFO_S] >= vfos[vfoHiLimit]) vfos[VFO_S] = vfos[vfoLoLimit];
              if (autoScanRate < 0 && vfos[VFO_S] <= vfos[vfoLoLimit]) vfos[VFO_S] = vfos[vfoHiLimit];
              break;
          break;
      }

      return (autoScanRate > 0) - (autoScanRate < 0);
}
  
#endif // USE_AUTOSCANNER

// End
