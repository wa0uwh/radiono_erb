// DialCal.cpp

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
#include "DialCal.h"
#include "debug.h"


#ifdef USE_DIAL_CAL


// Dial Calibration
byte vfoDialCalStash;

boolean dialCalEditMode = false;
long dialCalPP100M = 0;

// ###############################################################################
void toggleDialCal() {
#define DEBUG(x...)
#define DEBUG(x...) debugUnique(x)    // UnComment for Debug
    
    #ifdef USE_EDITIF
        if (editIfMode) return;    // Do Nothing if in Edit-IF-Mode
    #endif // USE_EDITIF
    if (inTx) return; // Do Nothing
    if (ritOn) return; // Do Nothing
    if (freqUnStable) return;  // Don't Change Modes if Frequency is UnStable
    
    if (!dialCalEditMode) { // Initiate Dial Cal Mode
        vfoDialCalStash = vfoActive;
        vfoActive = VFO_C;
        vfos[vfoActive] = vfos[vfoDialCalStash];
        dialCalEditMode = true;
    }
    else { // Compute Dial Cal PPB
        long Delta = (vfos[vfoActive] - vfos[vfoDialCalStash]);
        DEBUG("\nDelta= %ld", Delta);
        
        dialCalPP100M = ((100 * MHz) / vfos[vfoDialCalStash]);
        DEBUG("DialCalPP100M= %ld", dialCalPP100M);
        
        dialCalPP100M *= Delta;
        DEBUG("DialCalPP100M= %ld", dialCalPP100M);
        
        vfoActive = vfoDialCalStash;
        dialCalEditMode = false;
        
         
        // To be used as follows:
        //long delta = (vfos[vfoActive] / KHz) * dialCalPP100M; delta /= 100 * KHz; unsigned long Freq = vfos[vfoActive] + delta;
        //DEBUG("Freq= %lu", Freq);
        
    }
}

// ###############################################################################
void dialCalEditModeCancel() { // Cancel/Abort Dial Cal Mode
    if (dialCalEditMode) {
        vfoActive = vfoDialCalStash;
        dialCalEditMode = false;
    }
}

#endif // USE_DIAL_CAL

// End
