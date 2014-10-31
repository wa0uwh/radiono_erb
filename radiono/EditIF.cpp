// EditIF.cpp

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
#include "EditIF.h"
#include "debug.h"

boolean editIfMode = false;


// ###############################################################################
void editIf() {  // Set the IF Frequency
#define DEBUG(x ...)
//#define DEBUG(x ...) debugUnique(x)    // UnComment for Debug
    static int vfoEditIfStash = VFO_A;
    static boolean sbEditIfStash;

    DEBUG(P("%s/%d:"), __func__, __LINE__);
    
    
    #ifdef USE_PARK_CURSOR
        cursorDigitPosition = 0;
    #endif // USE_PARK_CURSOR

    if (editIfMode) {  // Save IF Freq, Reload Previous VFO
        vfos[vfoActive] += ritVal;
        vfoActive = vfoEditIfStash;
        isLSB = sbEditIfStash;
    }
    else {  // Save Current VFO, Load IF Freq
        vfoEditIfStash = vfoActive;
        sbEditIfStash = isLSB;
        vfoActive = isLSB ? VFO_L : VFO_U;
    }
    editIfMode = !editIfMode;  // Toggle Edit IF Mode    
    #ifdef USE_PARK_CURSOR
        cursorDigitPosition = 0;
    #endif // USE_PARK_CURSOR
    #ifdef USE_TUNE2500_MODE 
        tune2500Mode = 0;
    #endif // USE_TUNE2500_MODE
    ritOn = ritVal = 0;
}

// End
